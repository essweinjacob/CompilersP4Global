start
main {
    if [ 4 => 3 ] then {
        printf [ 0 ] .
        if [ 5 => 4 ] then {
            printf [ 1 ] .
            if [ 2 => 2 ] then {
                printf [ 2 ] .
                if [ 6 =< 7 ] then {
                    printf [ 3 ] .
                    if [ 1 : : 2 ] then {
                        printf [ 4 ] .
                    }.
                }.
            }.
        }.
    }.
} stop