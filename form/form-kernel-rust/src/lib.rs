// form-kernel-rust — C-ABI library surface.
//
// The same kernel that ships as the CLI binary, reachable from any host that
// can call C. The library is built only under the `cabi` feature (Android /
// embedded); the plain bin build pulls in nothing here, so main.rs compiles
// once (validate.sh / CI stay fast).
#![cfg(feature = "cabi")]

// Pull main.rs into the library as a sibling module. The bin target still
// uses main.rs as its own entry; we re-include it here as a module so its
// internal items (Value, run_source, sub-modules) are reachable from lib.rs
// without duplicating the kernel.
#[path = "main.rs"]
mod kernel;

// Re-export every public-within-crate item at the crate root. The sibling
// modules (formats, inductive, quotient) import paths like `crate::Kernel`
// and `crate::NodeID`. In the binary build those resolve because main.rs
// is the crate root; here the kernel lives one level down, so we surface
// its items back up.
#[allow(unused_imports)]
pub use kernel::*;

// ──────────────────────────────────────────────────────────────────────────
// C-ABI surface — the phone-native door. The SAME evaluator the CLI runs
// (kernel::run_source), reachable from any language that can call C: Android
// via JNI, embedded via FFI. No Python, no subprocess.
// ──────────────────────────────────────────────────────────────────────────
#[cfg(feature = "cabi")]
mod cabi {
    use crate::kernel;
    use std::ffi::{CStr, CString};
    use std::os::raw::c_char;
    use std::panic::{catch_unwind, AssertUnwindSafe};

    /// Evaluate a Form recipe source string; return the final value rendered as
    /// the kernel's `display()` text — the exact text the CLI prints. The
    /// returned heap C string MUST be freed with `form_eval_free`. A panic, a
    /// null pointer, or non-UTF-8 input comes back as a string starting "ERR:"
    /// (never a crash across the FFI boundary).
    #[no_mangle]
    pub extern "C" fn form_eval(src: *const c_char) -> *mut c_char {
        let out = catch_unwind(AssertUnwindSafe(|| {
            if src.is_null() {
                return "ERR: null source".to_string();
            }
            match unsafe { CStr::from_ptr(src) }.to_str() {
                Ok(s) => kernel::run_source(s).display(),
                Err(_) => "ERR: source not valid UTF-8".to_string(),
            }
        }))
        .unwrap_or_else(|_| "ERR: kernel panic".to_string());
        match CString::new(out) {
            Ok(c) => c.into_raw(),
            Err(_) => CString::new("ERR: nul byte in output").unwrap().into_raw(),
        }
    }

    /// Free a string returned by `form_eval`. Calling with null is a no-op.
    #[no_mangle]
    pub extern "C" fn form_eval_free(p: *mut c_char) {
        if !p.is_null() {
            unsafe { drop(CString::from_raw(p)) };
        }
    }

    /// JNI door — the SAME evaluator the C-ABI `form_eval` runs, named so the
    /// Android app binds it directly via `System.loadLibrary("form_kernel_rust")`
    /// + `external fun eval(src: String): String` on `com.coherence.sense.FormKernel`.
    /// No separate C shim, no second .so: the phone-native kernel is this one .so.
    /// jni owns the jstring↔String marshalling; a panic or bad input returns an
    /// "ERR:" string (never a crash across the boundary), mirroring form_eval.
    #[cfg(feature = "cabi")]
    #[no_mangle]
    pub extern "system" fn Java_com_coherence_sense_FormKernel_eval<'local>(
        mut env: jni::JNIEnv<'local>,
        _class: jni::objects::JClass<'local>,
        src: jni::objects::JString<'local>,
    ) -> jni::sys::jstring {
        let input: String = match env.get_string(&src) {
            Ok(s) => s.into(),
            Err(_) => {
                return env
                    .new_string("ERR: source not valid UTF-8")
                    .map(|s| s.into_raw())
                    .unwrap_or(std::ptr::null_mut());
            }
        };
        let out = catch_unwind(AssertUnwindSafe(|| kernel::run_source(&input).display()))
            .unwrap_or_else(|_| "ERR: kernel panic".to_string());
        env.new_string(out)
            .map(|s| s.into_raw())
            .unwrap_or(std::ptr::null_mut())
    }
}
