unsafe extern "C" {
    pub fn printVersion();
    pub fn ParseCacheFile(path: *const libc::c_char) -> u64;
    pub fn WriteCacheFile(buf_ptr: u64, path: *const libc::c_char);
    pub fn FreeCacheBuffer(buf_ptr: u64);
    pub fn AddToCache(but_ptr: u64, name: *const libc::c_char, name_length: u64, value: *const libc::c_char, value_length: u64);
    pub fn ReadFromCache(buf_ptr: u64, name: *const libc::c_char, name_length: u64, value_length: *mut u64) -> *const libc::c_char;
    pub fn CleanCache(buf_ptr: u64);
}
