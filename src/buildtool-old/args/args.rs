use crate::c;

pub fn print_version()
{
    unsafe
    {
        c::printVersion();
    }
}

pub fn print_help()
{
    println!("Usage: buildtool [OPTIONS]");
    println!("  -v, --version   Show version");
    println!("  -h, --help      Show this help message");
}
