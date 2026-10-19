//
// IGH EDUCATIVO - API endpoints used by the current LAN test build.
// GPL v3 or later.
//

#ifndef HEADER_EDUCATION_API_CONFIG_HPP
#define HEADER_EDUCATION_API_CONFIG_HPP

namespace Education
{
    // Android cannot use 127.0.0.1 to reach XAMPP running on the PC.
    // For this test build, the PC is reachable on the local Wi-Fi/LAN at
    // 192.168.1.115.
    static const char* const API_SYNC_URL =
        "http://192.168.1.115/igh-api/public/api/sync_stk.php";

    static const char* const API_ANSWER_URL =
        "http://192.168.1.115/igh-api/public/api/answer_stk.php";

    static const char* const API_RESULT_SYNC_URL =
        "http://192.168.1.115/igh-api/public/api/result_sync_stk.php";
}

#endif
