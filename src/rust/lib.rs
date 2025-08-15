use std::ffi::{CStr, CString};
use std::io::Write;
use std::process::{Command, Stdio};
use std::os::raw::c_char;

#[no_mangle]
pub extern "C" fn run_program(
    cmd: *const c_char,
    input: *const c_char,
    out_buf: *mut *mut c_char,
    err_buf: *mut *mut c_char,
) -> i32 {
    unsafe {
        let c_cmd = CStr::from_ptr(cmd).to_string_lossy();
        let c_input = CStr::from_ptr(input).to_string_lossy();

        // Split command into program + args
        let mut parts = c_cmd.split_whitespace();
        let program = match parts.next() {
            Some(p) => p,
            None => return -1,
        };
        let args: Vec<&str> = parts.collect();

        let mut child = match Command::new(program)
            .args(&args)
            .stdin(Stdio::piped())
            .stdout(Stdio::piped())
            .stderr(Stdio::piped())
            .spawn()
        {
            Ok(c) => c,
            Err(_) => return -1,
        };

        // stdin
        if let Some(mut stdin) = child.stdin.take() {
            let _ = stdin.write_all(c_input.as_bytes());
        }

        let output = match child.wait_with_output() {
            Ok(o) => o,
            Err(_) => return -1,
        };

        // stdout
        let stdout_c = CString::new(output.stdout).unwrap();
        *out_buf = stdout_c.into_raw();

        // stderr
        let stderr_c = CString::new(output.stderr).unwrap();
        *err_buf = stderr_c.into_raw();

        output.status.code().unwrap_or(-1)
    }
}

#[no_mangle]
pub extern "C" fn free_c_string(s: *mut c_char) {
    if !s.is_null() {
        unsafe { CString::from_raw(s); }
    }
}
