static const char* buggedCase = R"(
1CE9BD1C1C
1C1CBDE9BD
E9E9BDE955
55BD1CE91C
BD1CE9BD1C
)";

static const char* buggedGoal = R"(
E9BDBD
1C55E9
E9BD1C
)";

static vector<Problem> problems{
    {"Game", testBuffer, testProblem, testGoals},
    {"Failure", 6, case3, goal3},
    {
        "Test1a",
        3,
        case1,
        "1C E9\n",
    },
    {
        "Test1b",
        3,
        case1,
        "1C BD\n",
    },
    {"Test1c", 3, case1, "1C E9 E9\n"},
    {"Test1d", 4, case1, "1C E9 E9 E9\n"},
    {"Test1e", 4, case1, "1C E9 E9 1C\n"},
    {"Test1f", 5, case1, "1C E9 E9 1C\n"},
    {"Test2a", 3, case2, "55 1C 1C\n"},
    {"Test2b", 3, case2, "55 1C FF\n"},
    {"Test2c", 4, case2, "55 1C 1C\n"},
    {"Test2d", 4, case2, "BD 88 88\n"},
    {"Test2e", 5, case2, "88 E9 FF FF BD\n"},
    {"Test2f", 7, case2, "88 E9 FF FF BD\n"},
    {"Test3a", 3, case1, "1C E9\n1C E9 E9\n"},
    {"Test3b", 4, case1, "1C E9\n1C E9 E9\nE9 E9\n"},
    {"Test3c", 6, case1, "1C E9\n1C E9 E9\nE9 E9\nE9 55\n"},
};

const char* case1 =
    "1C 55 1C\n"
    "E9 E9 55\n"
    "55 1C E9\n";

const char* case2 =
    "55 BD 55 BD\n"
    "BD 55 BD 55\n"
    "1C FF FF 1C\n"
    "88 E9 7A 88\n";