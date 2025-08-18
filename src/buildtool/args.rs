unsafe extern "C" {
    fn printVersion();
}

pub fn print_version()
{
    unsafe
    {
        printVersion();
    }
}

pub fn print_help()
{
    println!("Usage: buildtool [OPTIONS]");
    println!("  -v, --version   Show version");
    println!("  -h, --help      Show this help message");
}
