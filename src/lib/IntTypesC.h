#pragma once

#if defined(__SIZEOF_INT128__)
    using Int128 = __int128;
    using UInt128 = unsigned __int128;
#else
    // FIXME: get fallback
    #error "__int128 isn't supported by this compiler"
#endif