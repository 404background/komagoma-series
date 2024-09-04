String controllerHTML =
"<!DOCTYPE html>\
<html>\
<head>\
    <meta charset=\"UTF-8\">\
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\
    <title>Controller</title>\
    <style>\
        body {\
            background-color: black;\
            height: 100%;\
            width: 100%;\
          }\
        .parent {\
            background-color: black;\
            display: grid;\
            grid-template-columns: repeat(3, 1fr);\
            grid-template-rows: repeat(3, 1fr);\
            grid-column-gap: 30px;\
            grid-row-gap: 30px;\
        }\
        \
        .div1 { grid-area: 1 / 1 / 2 / 2; }\
        .div2 { grid-area: 1 / 2 / 2 / 3; border: 50px solid transparent; border-bottom: 50px solid white; }\
        .div3 { grid-area: 1 / 3 / 2 / 4; }\
        .div4 { grid-area: 2 / 1 / 3 / 2; border: 50px solid transparent; border-right: 50px solid white; }\
        .div5 { grid-area: 2 / 2 / 3 / 3; border: 50px solid transparent; background-color: white; border-radius: 50%; }\
        .div6 { grid-area: 2 / 3 / 3 / 4; border: 50px solid transparent; border-left: 50px solid white; }\
        .div7 { grid-area: 3 / 1 / 4 / 2; }\
        .div8 { grid-area: 3 / 2 / 4 / 3; border: 50px solid transparent; border-top: 50px solid white; }\
        .div9 { grid-area: 3 / 3 / 4 / 4; }\
    </style>\
</head>\
<body>\
    <div class=\"parent\">\
        <a href=\"/LG\" class=\"div1\"></a>\
        <a href=\"/G\" class=\"div2\"></a>\
        <a href=\"/RG\" class=\"div3\"></a>\
        <a href=\"/L\" class=\"div4\"></a>\
        <a href=\"/N\" class=\"div5\"></a>\
        <a href=\"/R\" class=\"div6\"></a>\
        <a href=\"/LB\" class=\"div7\"></a>\
        <a href=\"/B\" class=\"div8\"></a>\
        <a href=\"/RB\" class=\"div9\"></a>\
        </div>\
</body>\
</html>\
";
