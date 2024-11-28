# CalculatorSoftware
This is a simple yet ultra-precise calculator made with GTK4 in C which has no precision loss. It's designed to show the full number. It has addition (`+`), subtraction (`-`), multiplication (`*`), division (`/`) & modulo (`%`) (The symbols in the brackets represent the operand) (Haven't added exponent yet).

Some of you might ask what will happen if we put `1/3` in the calculator. The truth is when you do such, it'll return `0.(3)` which is basically `0.3333333...`. You can also enter `.(3)*3` which will result `1`. It's that powerful.

To show you the power of this calculator, I've entered `2000/2001` which resulted
```
0.(99950024987506246876561719140429785107446276861569215392303848075962018990504747626186906546726636681659170414792603698150924537731134432783608195902048975512243878060969515242378810594702648675662168915542228885557221389305347326336831584207896051974012993503248375812093953023488255872063968015992003998000)
```
>[!IMPORTANT]
>Don't divide large numbers in this calculator as it'll make the calculator freeze (and that's its natural behaviour).
## Preview Image
![Capture](https://github.com/user-attachments/assets/af9ff304-b4a2-4cb7-b247-1d620b1dd57b)
## Extras
This calculator uses [strcal](https://github.com/Amirreza-Ipchi-Haq/strcal) for its logic.
