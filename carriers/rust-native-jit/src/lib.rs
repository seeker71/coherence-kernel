use std::fmt;

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
pub enum Route {
    Native,
    Deopt,
    Exception,
    Rewalk,
    Melt,
    Reject,
    Pending,
}

#[derive(Clone, Debug, Eq, PartialEq)]
pub struct SourceFrame {
    pub file: &'static str,
    pub line: u32,
    pub col: u32,
    pub span: u32,
    pub function: &'static str,
}

#[derive(Clone, Debug, Eq, PartialEq)]
pub struct RuntimeException {
    pub kind: &'static str,
    pub stack: Vec<SourceFrame>,
}

#[derive(Clone, Debug, Eq, PartialEq)]
pub enum CarrierOutcome {
    Native(i64),
    Deopt(i64),
    Exception(RuntimeException),
    Rewalk,
    Melt,
    Reject(&'static str),
    Pending(&'static str),
}

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
pub struct RouteInputs {
    pub guard_ok: bool,
    pub runtime_ok: bool,
    pub invalidated: bool,
    pub parity_ok: bool,
    pub stale: bool,
}

impl RouteInputs {
    pub const PASS: Self = Self {
        guard_ok: true,
        runtime_ok: true,
        invalidated: false,
        parity_ok: true,
        stale: false,
    };
}

#[derive(Clone, Debug, Eq, PartialEq)]
pub struct Payload {
    pub owner: &'static str,
    pub arch: &'static str,
    pub abi: &'static str,
    pub bytes: &'static [u8],
    pub source: SourceFrame,
}

impl Payload {
    pub fn validate(&self) -> Result<(), &'static str> {
        if self.owner != "form-backend" {
            return Err("foreign-owner");
        }
        if self.bytes.is_empty() {
            return Err("empty-payload");
        }
        if self.arch != std::env::consts::ARCH {
            return Err("arch-mismatch");
        }
        if self.source.span == 0 {
            return Err("missing-source-span");
        }
        Ok(())
    }
}

pub fn host_add1_payload() -> Payload {
    Payload {
        owner: "form-backend",
        arch: host_payload_arch(),
        abi: host_payload_abi(),
        bytes: host_add1_bytes(),
        source: SourceFrame {
            file: "observe/jit-live-execution-evidence.fk",
            line: 1,
            col: 1,
            span: 8,
            function: "form-add1",
        },
    }
}

#[cfg(target_arch = "aarch64")]
fn host_payload_arch() -> &'static str {
    "aarch64"
}

#[cfg(target_arch = "aarch64")]
fn host_payload_abi() -> &'static str {
    "aapcs64-args-vector"
}

#[cfg(target_arch = "aarch64")]
fn host_add1_bytes() -> &'static [u8] {
    // ldr x0, [x0]; add x0, x0, #2; ret
    &[
        0x00, 0x00, 0x40, 0xf9, 0x00, 0x08, 0x00, 0x91, 0xc0, 0x03, 0x5f, 0xd6,
    ]
}

#[cfg(target_arch = "x86_64")]
fn host_payload_arch() -> &'static str {
    "x86_64"
}

#[cfg(target_arch = "x86_64")]
fn host_payload_abi() -> &'static str {
    "sysv-args-vector"
}

#[cfg(target_arch = "x86_64")]
fn host_add1_bytes() -> &'static [u8] {
    // mov rax, [rdi]; add rax, 2; ret
    &[72, 139, 7, 72, 131, 192, 2, 195]
}

#[cfg(not(any(target_arch = "aarch64", target_arch = "x86_64")))]
fn host_payload_arch() -> &'static str {
    "unsupported"
}

#[cfg(not(any(target_arch = "aarch64", target_arch = "x86_64")))]
fn host_payload_abi() -> &'static str {
    "unsupported"
}

#[cfg(not(any(target_arch = "aarch64", target_arch = "x86_64")))]
fn host_add1_bytes() -> &'static [u8] {
    &[]
}

pub fn execute_add1(arg: i64) -> CarrierOutcome {
    execute_payload(&host_add1_payload(), &[arg], RouteInputs::PASS)
}

pub fn execute_payload(payload: &Payload, args: &[i64], route: RouteInputs) -> CarrierOutcome {
    if let Err(reason) = payload.validate() {
        return CarrierOutcome::Reject(reason);
    }
    if args.is_empty() {
        return CarrierOutcome::Reject("missing-args");
    }
    if route.stale {
        return CarrierOutcome::Melt;
    }
    if route.invalidated {
        return CarrierOutcome::Rewalk;
    }
    if !route.guard_ok {
        return CarrierOutcome::Deopt(args[0]);
    }
    if !route.runtime_ok {
        return CarrierOutcome::Exception(RuntimeException {
            kind: "runtime-fault",
            stack: vec![payload.source.clone()],
        });
    }
    if !route.parity_ok {
        return CarrierOutcome::Deopt(args[0]);
    }

    match execute_native_bytes(payload.bytes, args) {
        Ok(value) => CarrierOutcome::Native(value),
        Err(CarrierError::Unsupported) => CarrierOutcome::Pending("unsupported-host"),
        Err(CarrierError::InstallFailed) => CarrierOutcome::Reject("install-failed"),
    }
}

#[allow(dead_code)]
#[derive(Clone, Copy, Debug, Eq, PartialEq)]
enum CarrierError {
    Unsupported,
    InstallFailed,
}

#[cfg(unix)]
fn execute_native_bytes(bytes: &[u8], args: &[i64]) -> Result<i64, CarrierError> {
    unix_exec::execute(bytes, args)
}

#[cfg(not(unix))]
fn execute_native_bytes(_bytes: &[u8], _args: &[i64]) -> Result<i64, CarrierError> {
    Err(CarrierError::Unsupported)
}

#[cfg(unix)]
mod unix_exec {
    use super::CarrierError;
    use std::ffi::c_void;
    use std::ptr;

    const PROT_READ: i32 = 0x1;
    const PROT_WRITE: i32 = 0x2;
    const PROT_EXEC: i32 = 0x4;
    const MAP_PRIVATE: i32 = 0x02;
    #[cfg(target_os = "macos")]
    const MAP_ANON_FLAG: i32 = 0x1000;
    #[cfg(not(target_os = "macos"))]
    const MAP_ANON_FLAG: i32 = 0x20;

    unsafe extern "C" {
        fn mmap(
            addr: *mut c_void,
            len: usize,
            prot: i32,
            flags: i32,
            fd: i32,
            offset: isize,
        ) -> *mut c_void;
        fn mprotect(addr: *mut c_void, len: usize, prot: i32) -> i32;
        fn munmap(addr: *mut c_void, len: usize) -> i32;
    }

    #[cfg(target_os = "macos")]
    unsafe extern "C" {
        fn sys_icache_invalidate(start: *mut c_void, len: usize);
    }

    pub fn execute(bytes: &[u8], args: &[i64]) -> Result<i64, CarrierError> {
        if bytes.is_empty() {
            return Err(CarrierError::InstallFailed);
        }
        let len = bytes.len().max(4096);
        unsafe {
            let mem = mmap(
                ptr::null_mut(),
                len,
                PROT_READ | PROT_WRITE,
                MAP_PRIVATE | MAP_ANON_FLAG,
                -1,
                0,
            );
            if mem as isize == -1 {
                return Err(CarrierError::InstallFailed);
            }
            ptr::copy_nonoverlapping(bytes.as_ptr(), mem as *mut u8, bytes.len());
            flush_instruction_cache(mem, bytes.len());
            if mprotect(mem, len, PROT_READ | PROT_EXEC) != 0 {
                let _ = munmap(mem, len);
                return Err(CarrierError::InstallFailed);
            }
            let f: extern "C" fn(*const i64) -> i64 = std::mem::transmute(mem);
            let result = f(args.as_ptr());
            let _ = munmap(mem, len);
            Ok(result)
        }
    }

    #[cfg(target_os = "macos")]
    unsafe fn flush_instruction_cache(mem: *mut c_void, len: usize) {
        sys_icache_invalidate(mem, len);
    }

    #[cfg(not(target_os = "macos"))]
    unsafe fn flush_instruction_cache(_mem: *mut c_void, _len: usize) {}
}

impl fmt::Display for CarrierOutcome {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            CarrierOutcome::Native(v) => write!(f, "native {v}"),
            CarrierOutcome::Deopt(v) => write!(f, "deopt {v}"),
            CarrierOutcome::Exception(ex) => {
                let frame = ex.stack.first();
                if let Some(frame) = frame {
                    write!(
                        f,
                        "exception {} {}:{}:{}:{} {}",
                        ex.kind, frame.file, frame.line, frame.col, frame.span, frame.function
                    )
                } else {
                    write!(f, "exception {}", ex.kind)
                }
            }
            CarrierOutcome::Rewalk => write!(f, "rewalk"),
            CarrierOutcome::Melt => write!(f, "melt"),
            CarrierOutcome::Reject(reason) => write!(f, "reject {reason}"),
            CarrierOutcome::Pending(reason) => write!(f, "pending {reason}"),
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn executes_form_emitted_add1_payload() {
        assert_eq!(execute_add1(82), CarrierOutcome::Native(84));
    }

    #[test]
    fn routes_guard_failure_to_deopt_without_calling_native() {
        let outcome = execute_payload(
            &host_add1_payload(),
            &[82],
            RouteInputs {
                guard_ok: false,
                ..RouteInputs::PASS
            },
        );
        assert_eq!(outcome, CarrierOutcome::Deopt(82));
    }

    #[test]
    fn routes_runtime_failure_to_source_attributed_exception() {
        let outcome = execute_payload(
            &host_add1_payload(),
            &[82],
            RouteInputs {
                runtime_ok: false,
                ..RouteInputs::PASS
            },
        );
        match outcome {
            CarrierOutcome::Exception(ex) => {
                assert_eq!(ex.kind, "runtime-fault");
                assert_eq!(ex.stack.len(), 1);
                assert_eq!(ex.stack[0].function, "form-add1");
                assert_eq!(ex.stack[0].file, "observe/jit-live-execution-evidence.fk");
            }
            other => panic!("expected exception, got {other:?}"),
        }
    }

    #[test]
    fn routes_invalidation_and_stale_cache() {
        let rewalk = execute_payload(
            &host_add1_payload(),
            &[82],
            RouteInputs {
                invalidated: true,
                ..RouteInputs::PASS
            },
        );
        let melt = execute_payload(
            &host_add1_payload(),
            &[82],
            RouteInputs {
                stale: true,
                ..RouteInputs::PASS
            },
        );
        assert_eq!(rewalk, CarrierOutcome::Rewalk);
        assert_eq!(melt, CarrierOutcome::Melt);
    }

    #[test]
    fn rejects_foreign_owner() {
        let mut payload = host_add1_payload();
        payload.owner = "c-lowering";
        assert_eq!(
            execute_payload(&payload, &[82], RouteInputs::PASS),
            CarrierOutcome::Reject("foreign-owner")
        );
    }
}
