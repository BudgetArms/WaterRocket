let RenderToText = ['chart-temperature', 'chart-humidity', 'chart-pressure', 'chart-altitude', 'chart-acceleration'];
let ChartTitle = ['BME280 Temperature', 'BME280 Humidity', 'BME280 Pressure', 'BME280 Altitude', 'MPU9250 Acceleration'];
let yAxisTitleText = ['Temperature (Celsius)', 'Humidity (%)', 'Pressure (hPa)', 'Height (m)', 'Acceleration (m/s²)'];
let ColorChart = ['#059e8a', '#18009c', '#2f7ed8', '#696969', '#420420'];
let XHTTPName = ["/temperature", "/humidity", "/pressure", "/altitude", "/accelerationZ"];
let ChartName = ["ChartT", "ChartH", "ChartP", "ChartA", "ChartAc"];

// let ChartT = CreateChart(0);
// let ChartH = CreateChart(1);
// let ChartP = CreateChart(2);
let ChartA = CreateChart(3);
let ChartAc = CreateChart(4);



// Get the date
let timeNow = (new Date()).getTime();
let Launcher = document.getElementById("Launcher");
let ServoReady = document.getElementById("ServoReady");
let ReadyServoBool = false;
let Ready = false;

// Dont set this to zero, it will look extremely glitchy
// The sensors can not handle all those requests.
let delayPerRequest = 400;




function CreateChart(i) {

    return new Highcharts.Chart({
        chart: {
            renderTo: RenderToText[i]
        },
        title: {
            text: ChartTitle[i]
        },
        series: [{
            showInLegend: false,
            data: []
        }],
        plotOptions: {
            line: {
                animation: false,
                dataLabels: {
                    enabled: true
                }
            },
            series: {
                color: ColorChart[i]
            }
        },
        xAxis: {
            type: 'datetime',
            dateTimeLabelFormats: {
                second: '%H:%M:%S'
            }
        },
        yAxis: {
            min: undefined,
            title: {
                text: yAxisTitleText[i]
            }
        },
        credits: {
            enabled: false
        }
    });
}

setInterval(function() 
{

  if(!Ready) 
  {

    let xhttpReady = new XMLHttpRequest();
    xhttpReady.open("GET", "Ready", true);
    xhttpReady.onreadystatechange = function() 
    {
        if (this.readyState == 4 && this.status == 200) 
        {
          if(this.response== 1) 
          {
            Launcher.innerHTML = "READY TO LAUNCH";
            Launcher.style.color = "red";
            Ready = true;
          }
        }
    }
    xhttpReady.send();
  }


  
  if(!ReadyServoBool) 
  {
    let xhttpReadyServo = new XMLHttpRequest();
    xhttpReadyServo.open("GET", "parachuteOpen", true);
    xhttpReadyServo.onreadystatechange = function() 
    {
        if (this.readyState == 4 && this.status == 200) 
        {
          if(this.response == 1) 
          {
            console.log("Servo Fired");
            ServoReady.innerHTML = "Servo Fired";
            ServoReady.style.color = "red";
            ReadyServoBool = true;
          }
        }
    }
    xhttpReadyServo.send();
  }


  /*
  let xhttp = new XMLHttpRequest();
  xhttp.open("GET", XHTTPName[0], true);
  xhttp.onreadystatechange = function() 
  {
    if (this.readyState == 4 && this.status == 200) 
    {
      let x = (new Date()).getTime();
      let y = parseFloat(this.responseText);
      
      if (ChartT.series[0].data.length > 50) 
      {
        ChartT.series[0].addPoint([x, y], true, true, true);
      } 
      else 
      {
        ChartT.series[0].addPoint([x, y], true, false, true);
      }
    }
  }
  xhttp.send();


  let xhttp2 = new XMLHttpRequest();
  xhttp2.open("GET", XHTTPName[1], true);
  xhttp2.onreadystatechange = function() 
  {
    if (this.readyState == 4 && this.status == 200) 
    {
      let x = (new Date()).getTime();
      let y = parseFloat(this.responseText);
      
      if (ChartH.series[0].data.length > 50) 
      {
        ChartH.series[0].addPoint([x, y], true, true, true);
      } 
      else 
      {
        ChartH.series[0].addPoint([x, y], true, false, true);
      }
    }
  }
  xhttp2.send();


  let xhttp3 = new XMLHttpRequest();
  xhttp3.open("GET", XHTTPName[2], true);
  xhttp3.onreadystatechange = function() 
  {
    if (this.readyState == 4 && this.status == 200) 
    {
      let x = (new Date()).getTime();
      let y = parseFloat(this.responseText);
      
      if (ChartP.series[0].data.length > 50) 
      {
        ChartP.series[0].addPoint([x, y], true, true, true);
      } 
      else 
      {
        ChartP.series[0].addPoint([x, y], true, false, true);
      }
    }
  }
  xhttp3.send();
  */




  let xhttp4 = new XMLHttpRequest();
  xhttp4.open("GET", XHTTPName[3], true);
  xhttp4.onreadystatechange = function() 
  {
      if (this.readyState == 4 && this.status == 200) 
      {
          let x = (new Date()).getTime();
          let y = parseFloat(this.responseText);

          if (ChartA.series[0].data.length > 50) 
          {
              ChartA.series[0].addPoint([x, y], true, true, true);
          } 
          else 
          {
              ChartA.series[0].addPoint([x, y], true, false, true);
          }
      };
  }
  xhttp4.send();


  let xhttp5 = new XMLHttpRequest();
  xhttp5.open("GET", XHTTPName[4], true);
  xhttp5.onreadystatechange = function() 
  {
      if (this.readyState == 4 && this.status == 200) 
      {
          let x = (new Date()).getTime();
          let y = parseFloat(this.responseText);

          if (ChartAc.series[0].data.length > 50) 
          {
              ChartAc.series[0].addPoint([x, y], true, true, true);
          } 
          else 
          {
              ChartAc.series[0].addPoint([x, y], true, false, true);
          }
      };
  }
  xhttp5.send();

    
}, delayPerRequest);