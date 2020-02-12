import parser, calc, math, tables, os, times

when declared(commandLineParams):
    if commandLineParams().len > 0:
        let time = cpuTime()
        var
            f_str = commandLineParams()[0]
            f = fstr.parse
            f_sim = f.simplify
            dx_f = f_sim.derive
            dx_f_sim = dx_f.simplify

        var n = cpuTime() - time;
        echo "Eingabe: \t\t\t" & f_str
        echo "Parsed: \t\t\t" & $f
        echo "Simplified: \t\t\t" & $f_sim
        echo "Wert bei x = 2: \t\t" & $(to_node f.eval({0: 2.0}.to_table))
        echo "Derivative: \t\t\t" & $dx_f_sim
        echo "Time taken: ", int (n * 1000), " ms"