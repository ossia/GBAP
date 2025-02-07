/*



arguments: fgred fggreen fgblue bgred bggreen bgblue dialred dialgreen dialblue

*/

sketch.default2d();

var vbrgb = [0.,0.,0.,0.];
var vfrgb = [0.5,0.5,0.5,1.];
var vrgb2 = [0.2,0.2,0.99,0.7];

var normalizeV = false;
var rollOffV = 1;
var gainV = 1;

var numCascade = 0;
var cascadeList = [];

var posX = 0.5;
var posY = 0.5;
var cursorSizeX = 0.04;
var cursorSizeY = 0.04;

var nSinksX = 5;
var nSinksY = 4;
var volumes = [];
var nSinks = (nSinksX*nSinksY);
while(nSinks--) volumes.push(0);

var sinkSizeX = 0.05;
var sinkSizeY = 0.05;
// update this when values change
var intervX = (nSinksX-1)?(1-sinkSizeX)/(nSinksX-1)-sinkSizeX : 0;
var intervY = (nSinksY-1)?(1-sinkSizeY)/(nSinksY-1)-sinkSizeY : 0;


// process arguments
if (jsarguments.length>1)
    nSinksX = jsarguments[1]/255.;
if (jsarguments.length>2)
    nSinksY = jsarguments[2]/255.;
if (jsarguments.length>3)
    sinkSizeX = jsarguments[3]/255.;
if (jsarguments.length>4)
    sinkSizeY = jsarguments[4]/255.;
if (jsarguments.length>5)
    cursorSizeX = jsarguments[5]/255.;
if (jsarguments.length>6)
    cursorSizeY = jsarguments[6]/255.;
if (jsarguments.length>7)
    vfrgb[0] = jsarguments[7]/255.;
if (jsarguments.length>8)
    vfrgb[1] = jsarguments[8]/255.;
if (jsarguments.length>9)
    vfrgb[2] = jsarguments[9]/255.;
if (jsarguments.length>10)
    vfrgb[3] = jsarguments[10]/255.;

draw();

function draw()
{
    var ratio = (box.rect[2]-box.rect[0])/(box.rect[3]-box.rect[1]);

    with (sketch) {
        // erase background
        glclearcolor(vbrgb[0],vbrgb[1],vbrgb[2],vbrgb[3]);
        glclear();

        // fill bgcircle
        glcolor(vfrgb);
        if (nSinksX == 1) {
            for (var j = 0; j<nSinksY;j++){
                moveto(0, j*(sinkSizeY+intervY)*2-1+sinkSizeY);
                cube(2*ratio,sinkSizeY,0);
            }
        } else if (nSinksY == 1) {
            for (var i = 0; i<nSinksX;i++){
                moveto((i*(sinkSizeX+intervX)*2-1+sinkSizeX)*ratio,0);
                cube(sinkSizeX*ratio,2,0);
            }
        } else {
            for (var i = 0; i<nSinksX;i++){
                for (var j = 0; j<nSinksY;j++){
                    moveto((i*(sinkSizeX+intervX)*2-1+sinkSizeX)*ratio,
                           j*(sinkSizeY+intervY)*2-1+sinkSizeY);
                    cube(sinkSizeX*ratio,sinkSizeY,0);
                }
            }
        }
        glcolor(vrgb2);
        moveto((posX*2-1)*ratio, 1-2*posY);
        cube(cursorSizeX*ratio, cursorSizeY, 0);
    }
}

function compute(){
    // find modules of bounds
    var minX=0, ModmX=0, maxX=1, ModMX=0, minY=0, ModmY=0, maxY=1, ModMY=0;
    if (nSinksX-1){
        minX = Math.min(Math.max(0,(posX-cursorSizeX/2)),0.999999);
        var ModminX = minX/(sinkSizeX+intervX);
        ModmX = Math.floor(ModminX)*2;
        if (ModminX-ModmX/2 >= sinkSizeX/(sinkSizeX+intervX)) ModmX++;
        maxX = Math.min(Math.max(0,(posX+cursorSizeX/2)),0.999999);
        var ModmaxX = maxX/(sinkSizeX+intervX);
        ModMX = Math.floor(ModmaxX)*2;
        if (ModmaxX-ModMX/2 >= sinkSizeX/(sinkSizeX+intervX)) ModMX++;
    } //calculates the cursor position according to the number of sinks/spaces preceding bounds on the X axis.

    if (nSinksY-1){
        minY = Math.min(Math.max(0,(posY-cursorSizeY/2)),0.999999);
        var ModminY = minY/(sinkSizeY+intervY);
        ModmY = Math.floor(ModminY)*2;
        if (ModminY-ModmY/2 >= sinkSizeY/(sinkSizeY+intervY)) ModmY++;
        maxY = Math.min(Math.max(0,(posY+cursorSizeY/2)),0.999999);
        var ModmaxY = maxY/(sinkSizeY+intervY);
        ModMY = Math.floor(ModmaxY)*2;
        if (ModmaxY-ModMY/2 >= sinkSizeY/(sinkSizeY+intervY)) ModMY++;
    } //calculates the cursor position according to the number of sinks/spaces preceding bounds on the Y axis.

    var nSinks = (nSinksX*nSinksY);
    while(nSinks--) volumes[nSinks] = 0;
    for (var y = ModmY; y<=ModMY;y++){
        for (var x = ModmX; x<=ModMX;x++){
            var modType = x%2+2*(y%2);
            var area = 0;
            var center = 0.5;
            var Xs, Xe, Ys, Ye;
            switch (modType){

                case 0: //assigns the value to a sink covered by our cursor
                    Xs = Math.max(minX,(sinkSizeX+intervX)*x/2);
                    Xe = Math.min(((sinkSizeX+intervX)*x+2*sinkSizeX)/2, maxX);
                    Ys = Math.max(minY,(sinkSizeY+intervY)*y/2);
                    Ye = Math.min(((sinkSizeY+intervY)*y+2*sinkSizeY)/2, maxY);
                    area = (Xe - Xs) / sinkSizeX * (Ye - Ys ) / sinkSizeY;
                    volumes[x/2+y/2*nSinksX] += area;
                    break;

                case 1: //assigns the value of two sinks between which our cursor lies on the Y axis
                    if (nSinksX-1){
                        Xm = ((sinkSizeX+intervX)*(x-1)+2*sinkSizeX)/2;
                        Xs = Math.max(minX,((sinkSizeX+intervX)*(x-1)+2*sinkSizeX)/2);
                        Xe = Math.min((sinkSizeX+intervX)*Math.floor((x+1)/2), maxX);
                        Ys = Math.max(minY,(sinkSizeY+intervY)*y/2);
                        Ye = Math.min(((sinkSizeY+intervY)*y+2*sinkSizeY)/2, maxY);
                        area = (Xe - Xs) / intervX * (Ye - Ys ) / sinkSizeY;
                        center = ((Xe + Xs) / 2 - Xm) / intervX;
                        volumes[Math.floor(x/2)+y/2*nSinksX] += (1-center)*area;
                        volumes[Math.ceil (x/2)+y/2*nSinksX] += center*area;
                    }
                    break;

                case 2: //assigns the value of two sinks between which our cursor lies on the X axis
                    if (nSinksY-1){
                        Ym = ((sinkSizeY+intervY)*(y-1)+2*sinkSizeY)/2;
                        Xs = Math.max(minX,(sinkSizeX+intervX)*x/2);
                        Xe = Math.min(((sinkSizeX+intervX)*x+2*sinkSizeX)/2, maxX);
                        Ys = Math.max(minY,((sinkSizeY+intervY)*(y-1)+2*sinkSizeY)/2);
                        Ye = Math.min((sinkSizeY+intervY)*Math.floor((y+1)/2), maxY);
                        area = (Xe - Xs) / sinkSizeX * (Ye - Ys ) / intervY;
                        center = ((Ye + Ys) / 2 - Ym) / intervY;
                        volumes[x/2+Math.floor(y/2)*nSinksX] += (1-center)*area;
                        volumes[x/2+Math.ceil(y/2)*nSinksX] += center*area;
                    }
                    break;

                case 3://assigns the value of four sinks between which our cursor lies.
                    if (nSinksY-1 + nSinksX-1){
                        Xm = ((sinkSizeX+intervX)*(x-1)+2*sinkSizeX)/2;
                        Ym = ((sinkSizeY+intervY)*(y-1)+2*sinkSizeY)/2;
                        Xs = Math.max(minX,((sinkSizeX+intervX)*(x-1)+2*sinkSizeX)/2);
                        Xe = Math.min((sinkSizeX+intervX)*Math.floor((x+1)/2), maxX);
                        Ys = Math.max(minY,((sinkSizeY+intervY)*(y-1)+2*sinkSizeY)/2);
                        Ye = Math.min((sinkSizeY+intervY)*Math.floor((y+1)/2), maxY);
                        area = (Xe - Xs) / intervX * (Ye - Ys ) / intervY;
                        var centerX = ((Xe + Xs) / 2 - Xm) / intervX;
                        var centerY = ((Ye + Ys) / 2 - Ym) / intervY;
                        volumes[Math.floor(x/2)+Math.floor(y/2)*nSinksX] += (1-centerX)*(1-centerY)*area;
                        volumes[Math.ceil(x/2)+Math.floor(y/2)*nSinksX]  += centerX*(1-centerY)*area;
                        volumes[Math.floor(x/2)+Math.ceil(y/2)*nSinksX]  += (1-centerX)*centerY*area;
                        volumes[Math.ceil(x/2)+Math.ceil(y/2)*nSinksX]   += centerX*centerY*area;
                    }
                    break; //assigns the value of four sinks between which our cursor lies.
            }
        }
    }
    //var max = normalize(volumes)
    //post(volumes)

    volumes = rollOffArray(volumes); //apply rolloffV

    if (normalizeV) volumes = normalizeArray(volumes);

    if (cascadeList.length > 0) volumes = mult(volumes, cascadeList[numCascade]);

    volumes = mult(volumes, gainV);

    bang();

    //msg_list(volumes);
    //msg_list([posX, posY, minX, ModmX, ModmY, ModMX, ModMY, ]);
}

function bang()
{
    draw();
    refresh();
    outlet(0,volumes);
}

function msg_float(v)
{
    //val = [v,v]; //Math.min(Math.max(-1,v),1);
    //notifyclients();
    bang();
}

function msg_list(v)
{
    //cascadeList = v;
    //notifyclients();
    bang();
}

function list()
{
    cascadeList = arguments;
    //notifyclients();
    compute();
    bang();
}

function systemNumber(v)
{
    numCascade = v - 1;
    //notifyclients();
    compute();
    bang();
}


function normalize(v)
{
    normalizeV = v;
    //notifyclients();
    compute();
    bang();
}

function rollOff(v)
{
    rollOffV = Math.exp(v / 8.6858);
    post (v, rollOffV, "\n")
    //notifyclients();
    compute();
    bang();
}


function pos(X, Y)
{
    posX = Math.min(Math.max(0.,X),1.);
    posY = Math.min(Math.max(0.,Y),1.);
    //notifyclients();
    compute();
    bang();
}

function gain(v)
{
    gainV = Math.min(Math.max(0.,v),4.);
    //notifyclients();
    compute();
    bang();
}

function numSinks(X, Y)
{
    nSinksX = X;
    nSinksY = Y;
    intervX = (nSinksX-1)?(1-sinkSizeX)/(nSinksX-1)-sinkSizeX : 0;
    intervY = (nSinksY-1)?(1-sinkSizeY)/(nSinksY-1)-sinkSizeY : 0;
    if (nSinksX==1) {cursorSizeX =1; sinkSizeX = 1; }
    if (nSinksY==1) {cursorSizeY =1; sinkSizeY = 1; }
    nSinks = (nSinksX*nSinksY);
    volumes = [];
    while(nSinks--) volumes.push(0);
    //notifyclients();
    compute();
    bang();
}

function sinkSize(X, Y)
{
    if (nSinksX-1) sinkSizeX = X;
    if (nSinksY-1) sinkSizeY = Y;
    if (nSinksX-1) intervX = (1-sinkSizeX)/(nSinksX-1)-sinkSizeX;
    if (nSinksY-1) intervY = (1-sinkSizeY)/(nSinksY-1)-sinkSizeY;
    //notifyclients();
    compute();
    bang();
}

function cursorSize(X, Y)
{
    if (nSinksX-1) cursorSizeX = X;
    if (nSinksY-1) cursorSizeY = Y;
    //notifyclients();
    compute();
    bang();
}

function set(v)
{
    val = v; //Math.min(Math.max(-1,v),1);
    //notifyclients();
    draw();
    refresh();
}

function fsaa(v)
{
    sketch.fsaa = v;
    bang();
}

function frgb(r,g,b)
{
    vfrgb[0] = r/255.;
    vfrgb[1] = g/255.;
    vfrgb[2] = b/255.;
    draw();
    refresh();
}

function rgb2(r,g,b)
{
    vrgb2[0] = r/255.;
    vrgb2[1] = g/255.;
    vrgb2[2] = b/255.;
    draw();
    refresh();
}

function brgb(r,g,b)
{
    vbrgb[0] = r/255.;
    vbrgb[1] = g/255.;
    vbrgb[2] = b/255.;
    draw();
    refresh();
}

function setvalueof(v)
{
    msg_float(v);
}

function getvalueof()
{
    return val;
}

// all mouse events are of the form:
// onevent <x>, <y>, <button down>, <cmd(PC ctrl)>, <shift>, <capslock>, <option>, <ctrl(PC rbutton)>
// if you don't care about the additonal modifiers args, you can simply leave them out.
// one potentially confusing thing is that mouse events are in absolute screen coordinates,
// with (0,0) as left top, and (width,height) as right, bottom, while drawing
// coordinates are in relative world coordinates, with (0,0) as the center, +1 top, -1 bottom,
// and x coordinates using a uniform scale based on the y coordinates. to convert between screen
// and world coordinates, use sketch.screentoworld(x,y) and sketch.worldtoscreen(x,y,z).

function onclick(x,y,but,cmd,shift,capslock,option,ctrl)
{
    // cache mouse position for tracking delta movements
    posX = (nSinksX-1) ? Math.min(Math.max(0,(x/(box.rect[2]-box.rect[0]))),1) : 0.5;
    posY = (nSinksY-1) ? Math.min(Math.max(0,(y/(box.rect[3]-box.rect[1]))),1) : 0.5;
    compute();
}
onclick.local = 1; //private. could be left public to permit "synthetic" events

function ondrag(x,y,but,cmd,shift,capslock,option,ctrl)
{
    var dx, dy;
    // calculate delta movements
    if (nSinksX-1) dx = (x/(box.rect[2]-box.rect[0])) - posX;
    if (nSinksY-1) dy = (y/(box.rect[3]-box.rect[1])) - posY;
    if (shift) {
        // fine tune if shift key is down
        if (nSinksX-1) {
            cursorSizeX += dx*0.05;
            cursorSizeX = Math.min(Math.max(0.01,cursorSizeX),0.99)
        }
        if (nSinksY-1) {
            cursorSizeY -= dy*0.05;
            cursorSizeY = Math.min(Math.max(0.01,cursorSizeY),0.99)
        }
    } else {
        posX = (nSinksX-1) ? Math.min(Math.max(0,(x/(box.rect[2]-box.rect[0]))),1) : 0.5;
        posY = (nSinksY-1) ? Math.min(Math.max(0,(y/(box.rect[3]-box.rect[1]))),1) : 0.5;
    }
    compute()
}
ondrag.local = 1; //private. could be left public to permit "synthetic" events

function ondblclick(x,y,but,cmd,shift,capslock,option,ctrl)
{
    posX = (nSinksX-1) ? Math.min(Math.max(0,(x/(box.rect[2]-box.rect[0]))),1) : 0.5;
    posY = (nSinksY-1) ? Math.min(Math.max(0,(y/(box.rect[3]-box.rect[1]))),1) : 0.5;
    compute()
}
ondblclick.local = 1; //private. could be left public to permit "synthetic" events


function onresize(w,h)
{
    draw();
    refresh();
}
onresize.local = 1; //private

function rollOffArray(arr){
  var len = arr.length, sum = 0;
  while (len--) {
    sum += arr[len];
  }

  if (sum !=0){
    len = arr.length;
      while (len--) {
          arr[len] = Math.pow((arr[len] / sum), rollOffV/2);
          //arr[len] = arr[len] / sum;
      }
    }
  return arr;
};

function normalizeArray(arr) {
  var len = arr.length, max = 0;
  while (len--) {
    if (arr[len] > max) {
      max = arr[len];
    }
  }
  if (max !=0){
    len = arr.length;
      while (len--) {
          arr[len] = arr[len] / max;
      }
    }
  return arr;
};

function mult(arr, scal) {
  var len = arr.length;
  while (len--)
      arr[len] = arr[len] *= scal;
  return arr;
};
