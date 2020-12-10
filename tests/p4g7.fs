start
let x : 0
let y : 1
let z : 2
main {
    iter [ x =< 100 ] {
        y = 0 .
        z = 0 .
        iter [ y =< 10 ] {
            y = y + 1 .
            z = x + 1 .
            printf [ z ] .
        }.
        x = x + 10 .
    } .
} stop