use std::borrow::Cow;
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

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
pub struct FaultExit {
    pub code: i64,
    pub kind: &'static str,
}

static JIT_FAULTS: [FaultExit; 3] = [
    FaultExit {
        code: 101,
        kind: "bounds",
    },
    FaultExit {
        code: 102,
        kind: "null-ref",
    },
    FaultExit {
        code: 103,
        kind: "div-by-zero",
    },
];

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
    pub arity: usize,
    pub bytes: Cow<'static, [u8]>,
    pub source: SourceFrame,
    pub faults: &'static [FaultExit],
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
        arity: 1,
        bytes: Cow::Borrowed(host_add1_bytes()),
        source: SourceFrame {
            file: "observe/jit-live-execution-evidence.fk",
            line: 1,
            col: 1,
            span: 8,
            function: "form-add1",
        },
        faults: &[],
    }
}

pub fn host_checked_div_payload() -> Payload {
    Payload {
        owner: "form-backend",
        arch: host_payload_arch(),
        abi: host_payload_abi(),
        arity: 2,
        bytes: Cow::Borrowed(host_checked_div_bytes()),
        source: SourceFrame {
            file: "observe/jit-rust-carrier-checked-div.fk",
            line: 1,
            col: 1,
            span: 16,
            function: "form-checked-div",
        },
        faults: &[FaultExit {
            code: 103,
            kind: "div-by-zero",
        }],
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

#[cfg(target_arch = "aarch64")]
fn host_checked_div_bytes() -> &'static [u8] {
    // ldr x1, [x0]; ldr x2, [x0,#8]; cbz x2, fault; sdiv x0,x1,x2; ret; mov x0,#103; ret
    &[
        0x01, 0x00, 0x40, 0xf9, 0x02, 0x04, 0x40, 0xf9, 0x62, 0x00, 0x00, 0xb4, 0x20, 0x0c, 0xc2,
        0x9a, 0xc0, 0x03, 0x5f, 0xd6, 0xe0, 0x0c, 0x80, 0xd2, 0xc0, 0x03, 0x5f, 0xd6,
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

#[cfg(target_arch = "x86_64")]
fn host_checked_div_bytes() -> &'static [u8] {
    // mov rax,[rdi]; mov rcx,[rdi+8]; cmp rcx,0; je fault; cqo; idiv rcx; ret; mov rax,103; ret
    &[
        72, 139, 7, 72, 139, 79, 8, 72, 131, 249, 0, 116, 6, 72, 153, 72, 247, 249, 195, 72, 199,
        192, 103, 0, 0, 0, 195,
    ]
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

#[cfg(not(any(target_arch = "aarch64", target_arch = "x86_64")))]
fn host_checked_div_bytes() -> &'static [u8] {
    &[]
}

pub fn execute_add1(arg: i64) -> CarrierOutcome {
    execute_payload(&host_add1_payload(), &[arg], RouteInputs::PASS)
}

pub fn execute_checked_div(numerator: i64, denominator: i64) -> CarrierOutcome {
    execute_payload(
        &host_checked_div_payload(),
        &[numerator, denominator],
        RouteInputs::PASS,
    )
}

#[derive(Clone, Debug, Eq, PartialEq)]
pub enum JitOp {
    LoadArg(usize),
    AddImm(i32),
    CheckedDivArg(usize),
    CheckedArrayGet {
        ptr_arg: usize,
        index_arg: usize,
        len_arg: usize,
    },
    CheckedFieldLoad {
        ptr_arg: usize,
        slot: usize,
    },
}

#[derive(Clone, Debug, Eq, PartialEq)]
pub struct HotFunction {
    pub arity: usize,
    pub source: SourceFrame,
    pub ops: Vec<JitOp>,
}

pub fn add1_hot_function() -> HotFunction {
    HotFunction {
        arity: 1,
        source: SourceFrame {
            file: "observe/jit-live-execution-evidence.fk",
            line: 1,
            col: 1,
            span: 8,
            function: "form-add1",
        },
        ops: vec![JitOp::LoadArg(0), JitOp::AddImm(2)],
    }
}

pub fn checked_div_hot_function() -> HotFunction {
    HotFunction {
        arity: 2,
        source: SourceFrame {
            file: "observe/jit-rust-carrier-checked-div.fk",
            line: 1,
            col: 1,
            span: 16,
            function: "form-checked-div",
        },
        ops: vec![JitOp::LoadArg(0), JitOp::CheckedDivArg(1)],
    }
}

pub fn checked_array_get_hot_function() -> HotFunction {
    HotFunction {
        arity: 3,
        source: SourceFrame {
            file: "observe/jit-rust-carrier-checked-array.fk",
            line: 1,
            col: 1,
            span: 19,
            function: "form-checked-array-get",
        },
        ops: vec![JitOp::CheckedArrayGet {
            ptr_arg: 0,
            index_arg: 1,
            len_arg: 2,
        }],
    }
}

pub fn checked_field_load_hot_function(slot: usize) -> HotFunction {
    HotFunction {
        arity: 1,
        source: SourceFrame {
            file: "observe/jit-rust-carrier-checked-field.fk",
            line: 1,
            col: 1,
            span: 19,
            function: "form-checked-field-load",
        },
        ops: vec![JitOp::CheckedFieldLoad { ptr_arg: 0, slot }],
    }
}

pub fn compile_hot_function(function: &HotFunction) -> Result<Payload, &'static str> {
    validate_hot_function(function)?;
    let bytes = compile_ops(&function.ops)?;
    Ok(Payload {
        owner: "form-backend",
        arch: host_payload_arch(),
        abi: host_payload_abi(),
        arity: function.arity,
        bytes: Cow::Owned(bytes),
        source: function.source.clone(),
        faults: &JIT_FAULTS,
    })
}

pub fn execute_hot_function(function: &HotFunction, args: &[i64]) -> CarrierOutcome {
    match compile_hot_function(function) {
        Ok(payload) => execute_payload(&payload, args, RouteInputs::PASS),
        Err(reason) => CarrierOutcome::Reject(reason),
    }
}

pub fn execute_checked_array_get(values: &[i64], index: i64) -> CarrierOutcome {
    let args = [values.as_ptr() as isize as i64, index, values.len() as i64];
    execute_hot_function(&checked_array_get_hot_function(), &args)
}

pub fn execute_null_array_get(index: i64, len: i64) -> CarrierOutcome {
    let args = [0, index, len];
    execute_hot_function(&checked_array_get_hot_function(), &args)
}

pub fn execute_checked_field_load(fields: &[i64], slot: usize) -> CarrierOutcome {
    let args = [fields.as_ptr() as isize as i64];
    execute_hot_function(&checked_field_load_hot_function(slot), &args)
}

pub fn execute_null_field_load(slot: usize) -> CarrierOutcome {
    execute_hot_function(&checked_field_load_hot_function(slot), &[0])
}

pub fn execute_payload(payload: &Payload, args: &[i64], route: RouteInputs) -> CarrierOutcome {
    if let Err(reason) = payload.validate() {
        return CarrierOutcome::Reject(reason);
    }
    if args.len() < payload.arity {
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

    match execute_native_bytes(payload.bytes.as_ref(), args) {
        Ok(value) => fault_outcome(payload, value).unwrap_or(CarrierOutcome::Native(value)),
        Err(CarrierError::Unsupported) => CarrierOutcome::Pending("unsupported-host"),
        Err(CarrierError::InstallFailed) => CarrierOutcome::Reject("install-failed"),
    }
}

fn fault_outcome(payload: &Payload, value: i64) -> Option<CarrierOutcome> {
    payload
        .faults
        .iter()
        .find(|fault| fault.code == value)
        .map(|fault| {
            CarrierOutcome::Exception(RuntimeException {
                kind: fault.kind,
                stack: vec![payload.source.clone()],
            })
        })
}

fn validate_hot_function(function: &HotFunction) -> Result<(), &'static str> {
    if function.arity == 0 {
        return Err("missing-arity");
    }
    if function.ops.is_empty() {
        return Err("missing-ops");
    }
    if function.source.span == 0 {
        return Err("missing-source-span");
    }
    for op in &function.ops {
        match op {
            JitOp::LoadArg(index) | JitOp::CheckedDivArg(index) => {
                if *index >= function.arity {
                    return Err("arg-out-of-range");
                }
            }
            JitOp::CheckedArrayGet {
                ptr_arg,
                index_arg,
                len_arg,
            } => {
                if *ptr_arg >= function.arity
                    || *index_arg >= function.arity
                    || *len_arg >= function.arity
                {
                    return Err("arg-out-of-range");
                }
            }
            JitOp::CheckedFieldLoad { ptr_arg, slot: _ } => {
                if *ptr_arg >= function.arity {
                    return Err("arg-out-of-range");
                }
            }
            JitOp::AddImm(_) => {}
        }
    }
    Ok(())
}

#[cfg(target_arch = "aarch64")]
fn compile_ops(ops: &[JitOp]) -> Result<Vec<u8>, &'static str> {
    #[derive(Clone, Copy)]
    enum Branch {
        Cbz(u8),
        BCond(u8),
    }

    #[derive(Clone, Copy)]
    struct Patch {
        position: usize,
        code: i64,
        branch: Branch,
    }

    let mut out = Vec::new();
    let mut fault_patches = Vec::new();

    push_u32_le(&mut out, a64_mov_reg(9, 0));
    for op in ops {
        match *op {
            JitOp::LoadArg(index) => push_u32_le(&mut out, a64_ldr(0, 9, index)?),
            JitOp::AddImm(imm) => push_u32_le(&mut out, a64_add_imm(0, 0, imm)?),
            JitOp::CheckedDivArg(index) => {
                push_u32_le(&mut out, a64_ldr(10, 9, index)?);
                fault_patches.push(Patch {
                    position: out.len(),
                    code: 103,
                    branch: Branch::Cbz(10),
                });
                push_u32_le(&mut out, a64_cbz(10, 0)?);
                push_u32_le(&mut out, a64_sdiv(0, 0, 10));
            }
            JitOp::CheckedArrayGet {
                ptr_arg,
                index_arg,
                len_arg,
            } => {
                push_u32_le(&mut out, a64_ldr(11, 9, ptr_arg)?);
                fault_patches.push(Patch {
                    position: out.len(),
                    code: 102,
                    branch: Branch::Cbz(11),
                });
                push_u32_le(&mut out, a64_cbz(11, 0)?);
                push_u32_le(&mut out, a64_ldr(10, 9, index_arg)?);
                push_u32_le(&mut out, a64_ldr(12, 9, len_arg)?);
                push_u32_le(&mut out, a64_cmp(10, 12));
                fault_patches.push(Patch {
                    position: out.len(),
                    code: 101,
                    branch: Branch::BCond(2),
                });
                push_u32_le(&mut out, a64_bcond(2, 0)?);
                push_u32_le(&mut out, a64_ldr_indexed_x0_x11_x10());
            }
            JitOp::CheckedFieldLoad { ptr_arg, slot } => {
                push_u32_le(&mut out, a64_ldr(11, 9, ptr_arg)?);
                fault_patches.push(Patch {
                    position: out.len(),
                    code: 102,
                    branch: Branch::Cbz(11),
                });
                push_u32_le(&mut out, a64_cbz(11, 0)?);
                push_u32_le(&mut out, a64_ldr(0, 11, slot)?);
            }
        }
    }
    push_u32_le(&mut out, 0xd65f03c0);

    if !fault_patches.is_empty() {
        let fault_codes: Vec<i64> = fault_patches.iter().map(|patch| patch.code).collect();
        let fault_starts = append_fault_trailers_a64(&mut out, &fault_codes);
        for patch in fault_patches {
            let target = fault_start_for(patch.code, &fault_starts)?;
            let offset_words = ((target - patch.position) / 4) as i32;
            let word = match patch.branch {
                Branch::Cbz(rt) => a64_cbz(rt, offset_words)?,
                Branch::BCond(cond) => a64_bcond(cond, offset_words)?,
            };
            out[patch.position..patch.position + 4].copy_from_slice(&word.to_le_bytes());
        }
    }

    Ok(out)
}

#[cfg(target_arch = "aarch64")]
fn append_fault_trailers_a64(out: &mut Vec<u8>, codes: &[i64]) -> Vec<(i64, usize)> {
    let mut starts = Vec::new();
    for code in codes {
        let code = *code;
        if starts.iter().any(|(seen, _)| *seen == code) {
            continue;
        }
        starts.push((code, out.len()));
        push_u32_le(out, a64_movz(0, code as u16));
        push_u32_le(out, 0xd65f03c0);
    }
    starts
}

#[cfg(target_arch = "aarch64")]
fn a64_mov_reg(rd: u8, rn: u8) -> u32 {
    0xaa0003e0 | ((rn as u32) << 16) | rd as u32
}

#[cfg(target_arch = "aarch64")]
fn a64_ldr(rt: u8, rn: u8, index: usize) -> Result<u32, &'static str> {
    if index > 4095 {
        return Err("arg-offset-too-large");
    }
    Ok(0xf9400000 | ((index as u32) << 10) | ((rn as u32) << 5) | rt as u32)
}

#[cfg(target_arch = "aarch64")]
fn a64_add_imm(rd: u8, rn: u8, imm: i32) -> Result<u32, &'static str> {
    if !(0..=4095).contains(&imm) {
        return Err("unsupported-add-imm");
    }
    Ok(0x91000000 | ((imm as u32) << 10) | ((rn as u32) << 5) | rd as u32)
}

#[cfg(target_arch = "aarch64")]
fn a64_cbz(rt: u8, offset_words: i32) -> Result<u32, &'static str> {
    if !(-(1 << 18)..(1 << 18)).contains(&offset_words) {
        return Err("branch-out-of-range");
    }
    let imm19 = (offset_words as u32) & 0x7ffff;
    Ok(0xb4000000 | (imm19 << 5) | rt as u32)
}

#[cfg(target_arch = "aarch64")]
fn a64_sdiv(rd: u8, rn: u8, rm: u8) -> u32 {
    0x9ac00c00 | ((rm as u32) << 16) | ((rn as u32) << 5) | rd as u32
}

#[cfg(target_arch = "aarch64")]
fn a64_cmp(rn: u8, rm: u8) -> u32 {
    0xeb00001f | ((rm as u32) << 16) | ((rn as u32) << 5)
}

#[cfg(target_arch = "aarch64")]
fn a64_bcond(cond: u8, offset_words: i32) -> Result<u32, &'static str> {
    if !(-(1 << 18)..(1 << 18)).contains(&offset_words) {
        return Err("branch-out-of-range");
    }
    let imm19 = (offset_words as u32) & 0x7ffff;
    Ok(0x54000000 | (imm19 << 5) | cond as u32)
}

#[cfg(target_arch = "aarch64")]
fn a64_ldr_indexed_x0_x11_x10() -> u32 {
    0xf86a7960
}

#[cfg(target_arch = "aarch64")]
fn a64_movz(rd: u8, imm: u16) -> u32 {
    0xd2800000 | ((imm as u32) << 5) | rd as u32
}

#[cfg(target_arch = "aarch64")]
fn push_u32_le(out: &mut Vec<u8>, word: u32) {
    out.extend_from_slice(&word.to_le_bytes());
}

#[cfg(target_arch = "x86_64")]
fn compile_ops(ops: &[JitOp]) -> Result<Vec<u8>, &'static str> {
    #[derive(Clone, Copy)]
    struct Patch {
        position: usize,
        code: i64,
    }

    let mut out = Vec::new();
    let mut fault_patches = Vec::new();

    for op in ops {
        match *op {
            JitOp::LoadArg(index) => x64_load_arg_rax(&mut out, index)?,
            JitOp::AddImm(imm) => x64_add_imm(&mut out, imm),
            JitOp::CheckedDivArg(index) => {
                x64_load_arg_rcx(&mut out, index)?;
                out.extend_from_slice(&[72, 131, 249, 0, 116]);
                fault_patches.push(Patch {
                    position: out.len(),
                    code: 103,
                });
                out.push(0);
                out.extend_from_slice(&[72, 153, 72, 247, 249]);
            }
            JitOp::CheckedArrayGet {
                ptr_arg,
                index_arg,
                len_arg,
            } => {
                x64_load_arg_r8(&mut out, ptr_arg)?;
                out.extend_from_slice(&[77, 133, 192, 116]);
                fault_patches.push(Patch {
                    position: out.len(),
                    code: 102,
                });
                out.push(0);
                x64_load_arg_rcx(&mut out, index_arg)?;
                x64_load_arg_rdx(&mut out, len_arg)?;
                out.extend_from_slice(&[72, 57, 209, 115]);
                fault_patches.push(Patch {
                    position: out.len(),
                    code: 101,
                });
                out.push(0);
                out.extend_from_slice(&[73, 139, 4, 200]);
            }
            JitOp::CheckedFieldLoad { ptr_arg, slot } => {
                x64_load_arg_r8(&mut out, ptr_arg)?;
                out.extend_from_slice(&[77, 133, 192, 116]);
                fault_patches.push(Patch {
                    position: out.len(),
                    code: 102,
                });
                out.push(0);
                x64_load_field_rax(&mut out, slot)?;
            }
        }
    }
    out.push(195);

    if !fault_patches.is_empty() {
        let fault_codes: Vec<i64> = fault_patches.iter().map(|patch| patch.code).collect();
        let fault_starts = append_fault_trailers_x64(&mut out, &fault_codes);
        for patch in fault_patches {
            let target = fault_start_for(patch.code, &fault_starts)?;
            let rel = target as isize - (patch.position + 1) as isize;
            if !(-128..=127).contains(&rel) {
                return Err("branch-out-of-range");
            }
            out[patch.position] = rel as i8 as u8;
        }
    }

    Ok(out)
}

#[cfg(target_arch = "x86_64")]
fn append_fault_trailers_x64(out: &mut Vec<u8>, codes: &[i64]) -> Vec<(i64, usize)> {
    let mut starts = Vec::new();
    for code in codes {
        let code = *code;
        if starts.iter().any(|(seen, _)| *seen == code) {
            continue;
        }
        starts.push((code, out.len()));
        out.extend_from_slice(&[72, 199, 192]);
        out.extend_from_slice(&(code as i32).to_le_bytes());
        out.push(195);
    }
    starts
}

#[cfg(target_arch = "x86_64")]
fn x64_load_arg_rax(out: &mut Vec<u8>, index: usize) -> Result<(), &'static str> {
    if index == 0 {
        out.extend_from_slice(&[72, 139, 7]);
        return Ok(());
    }
    let offset = checked_disp8(index)?;
    out.extend_from_slice(&[72, 139, 71, offset]);
    Ok(())
}

#[cfg(target_arch = "x86_64")]
fn x64_load_arg_rcx(out: &mut Vec<u8>, index: usize) -> Result<(), &'static str> {
    if index == 0 {
        out.extend_from_slice(&[72, 139, 15]);
        return Ok(());
    }
    let offset = checked_disp8(index)?;
    out.extend_from_slice(&[72, 139, 79, offset]);
    Ok(())
}

#[cfg(target_arch = "x86_64")]
fn x64_load_arg_rdx(out: &mut Vec<u8>, index: usize) -> Result<(), &'static str> {
    if index == 0 {
        out.extend_from_slice(&[72, 139, 23]);
        return Ok(());
    }
    let offset = checked_disp8(index)?;
    out.extend_from_slice(&[72, 139, 87, offset]);
    Ok(())
}

#[cfg(target_arch = "x86_64")]
fn x64_load_arg_r8(out: &mut Vec<u8>, index: usize) -> Result<(), &'static str> {
    if index == 0 {
        out.extend_from_slice(&[76, 139, 7]);
        return Ok(());
    }
    let offset = checked_disp8(index)?;
    out.extend_from_slice(&[76, 139, 71, offset]);
    Ok(())
}

#[cfg(target_arch = "x86_64")]
fn x64_load_field_rax(out: &mut Vec<u8>, slot: usize) -> Result<(), &'static str> {
    let offset = slot.checked_mul(8).ok_or("field-offset-too-large")?;
    if offset == 0 {
        out.extend_from_slice(&[73, 139, 0]);
    } else if offset <= 127 {
        out.extend_from_slice(&[73, 139, 64, offset as u8]);
    } else if offset <= i32::MAX as usize {
        out.extend_from_slice(&[73, 139, 128]);
        out.extend_from_slice(&(offset as i32).to_le_bytes());
    } else {
        return Err("field-offset-too-large");
    }
    Ok(())
}

#[cfg(target_arch = "x86_64")]
fn checked_disp8(index: usize) -> Result<u8, &'static str> {
    let offset = index.checked_mul(8).ok_or("arg-offset-too-large")?;
    if offset > 127 {
        return Err("arg-offset-too-large");
    }
    Ok(offset as u8)
}

#[cfg(target_arch = "x86_64")]
fn x64_add_imm(out: &mut Vec<u8>, imm: i32) {
    if (-128..=127).contains(&imm) {
        out.extend_from_slice(&[72, 131, 192, imm as i8 as u8]);
    } else {
        out.extend_from_slice(&[72, 5]);
        out.extend_from_slice(&imm.to_le_bytes());
    }
}

fn fault_start_for(code: i64, starts: &[(i64, usize)]) -> Result<usize, &'static str> {
    starts
        .iter()
        .find(|(seen, _)| *seen == code)
        .map(|(_, start)| *start)
        .ok_or("missing-fault-trailer")
}

#[cfg(not(any(target_arch = "aarch64", target_arch = "x86_64")))]
fn compile_ops(_ops: &[JitOp]) -> Result<Vec<u8>, &'static str> {
    Err("unsupported-host")
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
    fn compiles_and_executes_add1_hot_function() {
        let payload = compile_hot_function(&add1_hot_function()).expect("compile add1");
        assert!(!payload.bytes.is_empty());
        assert_eq!(
            execute_payload(&payload, &[82], RouteInputs::PASS),
            CarrierOutcome::Native(84)
        );
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

    #[test]
    fn executes_checked_div_payload() {
        assert_eq!(execute_checked_div(84, 2), CarrierOutcome::Native(42));
    }

    #[test]
    fn compiles_and_executes_checked_div_hot_function() {
        assert_eq!(
            execute_hot_function(&checked_div_hot_function(), &[84, 2]),
            CarrierOutcome::Native(42)
        );
    }

    #[test]
    fn maps_native_div_zero_fault_to_source_exception() {
        match execute_checked_div(84, 0) {
            CarrierOutcome::Exception(ex) => {
                assert_eq!(ex.kind, "div-by-zero");
                assert_eq!(ex.stack.len(), 1);
                assert_eq!(ex.stack[0].function, "form-checked-div");
                assert_eq!(ex.stack[0].file, "observe/jit-rust-carrier-checked-div.fk");
            }
            other => panic!("expected div-by-zero exception, got {other:?}"),
        }
    }

    #[test]
    fn compiled_checked_div_maps_zero_to_source_exception() {
        match execute_hot_function(&checked_div_hot_function(), &[84, 0]) {
            CarrierOutcome::Exception(ex) => {
                assert_eq!(ex.kind, "div-by-zero");
                assert_eq!(ex.stack.len(), 1);
                assert_eq!(ex.stack[0].function, "form-checked-div");
            }
            other => panic!("expected div-by-zero exception, got {other:?}"),
        }
    }

    #[test]
    fn compiles_and_executes_checked_array_get() {
        let values = [11_i64, 22, 33];
        assert_eq!(
            execute_checked_array_get(&values, 1),
            CarrierOutcome::Native(22)
        );
    }

    #[test]
    fn compiled_array_get_maps_bounds_to_source_exception() {
        let values = [11_i64, 22, 33];
        match execute_checked_array_get(&values, 3) {
            CarrierOutcome::Exception(ex) => {
                assert_eq!(ex.kind, "bounds");
                assert_eq!(ex.stack.len(), 1);
                assert_eq!(ex.stack[0].function, "form-checked-array-get");
            }
            other => panic!("expected bounds exception, got {other:?}"),
        }
    }

    #[test]
    fn compiled_array_get_maps_null_to_source_exception() {
        match execute_null_array_get(0, 3) {
            CarrierOutcome::Exception(ex) => {
                assert_eq!(ex.kind, "null-ref");
                assert_eq!(ex.stack.len(), 1);
                assert_eq!(ex.stack[0].function, "form-checked-array-get");
            }
            other => panic!("expected null-ref exception, got {other:?}"),
        }
    }

    #[test]
    fn compiles_and_executes_checked_field_load() {
        let fields = [101_i64, 202, 303];
        assert_eq!(
            execute_checked_field_load(&fields, 2),
            CarrierOutcome::Native(303)
        );
    }

    #[test]
    fn compiled_field_load_maps_null_to_source_exception() {
        match execute_null_field_load(1) {
            CarrierOutcome::Exception(ex) => {
                assert_eq!(ex.kind, "null-ref");
                assert_eq!(ex.stack.len(), 1);
                assert_eq!(ex.stack[0].function, "form-checked-field-load");
            }
            other => panic!("expected null-ref exception, got {other:?}"),
        }
    }

    #[test]
    fn compiler_rejects_impossible_field_offset() {
        let err = compile_hot_function(&checked_field_load_hot_function(usize::MAX))
            .expect_err("oversized field slot must reject");
        assert!(err == "arg-offset-too-large" || err == "field-offset-too-large");
    }

    #[test]
    fn compiler_rejects_out_of_range_args() {
        let mut function = add1_hot_function();
        function.ops = vec![JitOp::LoadArg(1)];
        assert_eq!(compile_hot_function(&function), Err("arg-out-of-range"));
    }

    #[test]
    fn carrier_rejects_short_arg_frame_before_native_call() {
        let payload = compile_hot_function(&checked_div_hot_function()).expect("compile div");
        assert_eq!(
            execute_payload(&payload, &[84], RouteInputs::PASS),
            CarrierOutcome::Reject("missing-args")
        );
    }
}
