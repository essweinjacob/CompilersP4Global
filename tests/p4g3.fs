start
let x : 2
main {
    iter [3 =< 1 ] {
        printf [ 1 ] .
    } .
    iter [ x =< 5 ] {
        printf [ x ] .
        x = x + 1 .
    } .
    printf [ x ] .
} stop