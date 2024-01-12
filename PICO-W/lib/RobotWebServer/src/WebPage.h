const char *webPage = R"(
<!DOCTYPE html>
<html lang="en">

<head>
    <title>Hull Pixelbot Python-ish Editor</title>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap@4.3.1/dist/css/bootstrap.min.css"
        integrity="sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T" crossorigin="anonymous">
</head>

<body>
    <div class="container mb-2 mt-2">
        <h1 class="mb-4">&#128421;&#65039; Hull Pixelbot Python-ish Editor</h1>
    </div>
    <p>

    <div class="container mb-2 mt-2">
      <form method="POST" action="/save">
        <textarea id="codeTextarea" name="codeTextarea" style="font-family:monospace;" class="w-100" rows="20"
          cols="120"></textarea>
        <input type="submit" value="Save and run" class="btn btn-primary mt-2 mb-2 btn-block">
      </form>
    </div>

    <div class="container mb-2 mt-2" id="actionButtons">
        <div class="row justify-content-between">
            <div class="col">
                <a href="/run" class="btn btn-primary mb-2">Run</a>
            </div>
            <div class="col">
                <a href="/stop" class="btn btn-primary mb-2">Stop</a>
            </div>
            <div class="col">
                <button class="btn btn-primary mb-2" onclick="doSampleProgram();">Sample</button>
            </div>
            <div class="col">
                <button class="btn btn-primary mb-2" onclick="doClearProgram();">Clear</button>
            </div>
        </div>
    <div class="container mb-2 mt-2">
      <a href="http://www.hullpixelbot.com/HullOS%20Python-ish%20Specification.pdf" target="_blank">Python-ish Specification</a>
      </p>
  </div>
    </div>

    <script>
        var serial = null;

        let builtInProgramNo = 0;

        let builtInPrograms = [
`# Coloured flashing
forever
  red
  delay 5
  green
  delay 5
`,
`# Alarm 
forever
  sound 1000
  delay 5
  sound 2000
  delay 5
`,
`# square dance 
forever
  move 100
  turn 90
`,
`# distance light 
forever
  d = @distance
  if d < 100:
    red
    continue
  if d < 200:
    yellow
    continue
  green
`,
`# robot coward 
forever
  green
  d = @distance
  if d < 100:
    red
    move -100
`,
`# turn and run 
forever
  green
  d = @distance
  if d < 100:
    red
    turn 180
    move 100
`,
`# avoid obstacles 
forever
  green
  move
  d = @distance
  if d < 100:
    red
    turn 90
`,
`# spin the bottle
forever
  green
  if @distance < 100:
    a = @random*30
    a = a+360
    red
    turn a
`,
`# theramin
forever
  f=@distance*20
  f=f+1000
  sound f
`
    ];

    function doSampleProgram(){
        let input = document.getElementById('codeTextarea');
        input.value = builtInPrograms[builtInProgramNo];
        builtInProgramNo++;
        if(builtInProgramNo==builtInPrograms.length){
            builtInProgramNo=0;
        }
    }

    function doClearProgram() {
        let input = document.getElementById('codeTextarea');
        input.value = "";
    }
</script>
</body>

</html>
)";
