<?php

$csv = file_get_contents('../tests/result.csv');
$rows = explode("\n",$csv);
unset($rows[0]);
$data = array();
foreach($rows as $row) {
    $tmp = explode(";",$row);
    if(isset($tmp[3])) {
        if($tmp[3] == 'lz77') {
            $data[$tmp[0]][$tmp[3]][] = [intval($tmp[2]), floatval($tmp[9])];
        } elseif($tmp[3] == 'eac') {
            $data[$tmp[0]][$tmp[3]][] = [intval($tmp[1]), floatval($tmp[9])];
        }
    }
}

?>
<html lang="en">
  <head>
    <meta charset="utf-8" />
    <title>EAC results viewer</title>

    <!-- Styles -->
    <link href="/css/bootstrap.min.css" media="screen" rel="stylesheet" type="text/css">
    <link href="/css/prettify.css" media="screen" rel="stylesheet" type="text/css">
    <link href="/css/style.css" media="screen" rel="stylesheet" type="text/css">
    <link href="/css/bootstrap-responsive.min.css" media="screen" rel="stylesheet" type="text/css">
    <link href="/css/jquery.jqplot.min.css" media="screen" rel="stylesheet" type="text/css">
    <style type="text/css">
     .bootstrap-growl {
       box-shadow: 2px 2px 5px #888888;
     }
     #chart {
       margin-left: auto;
       margin-right: auto;
       width: 90%;
       height: 70%;
     }
    </style>

    <!-- Scripts -->
    <script type="text/javascript" src="/js/jquery.min.js"></script>
    <script type="text/javascript" src="/js/bootstrap.min.js"></script>
    <script type="text/javascript" src="/js/jquery.bootstrap-growl.min.js"></script>
    <!--[if lt IE 9]><script type="text/javascript" src="/js/html5.js"></script><![endif]-->
    <!--[if lt IE 9]><script type="text/javascript" src="/js/excanvas.js"></script><![endif]-->
    <script type="text/javascript" src="/js/jquery.jqplot.min.js"></script>
    <script type="text/javascript" src="/js/jqplot_plugins/jqplot.pointLabels.min.js"></script>
    <script type="text/javascript" src="/js/jqplot_plugins/jqplot.logAxisRenderer.js"></script>
    <script type="text/javascript" src="/js/jqplot_plugins/jqplot.canvasAxisLabelRenderer.js"></script>
    <script type="text/javascript" src="/js/jqplot_plugins/jqplot.highlighter.min.js"></script>
    <script type="text/javascript" src="/js/jqplot_plugins/jqplot.cursor.min.js"></script>
    <script type="text/javascript" src="/js/jqplot_plugins/jqplot.canvasAxisTickRenderer.js"></script>
  </head>
  <body>
    <div>
      <select id="file">
        <?php foreach($data as $file => $series) { ?>
          <option value="<?=$file?>"><?=$file?></option>
        <?php } ?>
      </select>
      <button class="btn button-reset" type="button">Reset zoom</button>
    </div>
    <div id="chart"></div>
    
    <script type="text/javascript">
     var data = <?=json_encode($data)?>;
     $(document).ready(function(){
         var s1 = data[Object.keys(data)[0]].lz77;
         var s2 = data[Object.keys(data)[0]].eac;
         plot = $.jqplot('chart',[s1,s2],{
             title: 'Compression ratio',
             seriesDefaults:{
                 rendererOptions: {
                 },
             },
             series:[
                 {label:'LZ77'},
                 {label:'EAC'}
             ],
             axes: {
                 xaxis: {
                     label: 'Window size / Block size',
                     renderer: $.jqplot.LogAxisRenderer,
                     ticks: [ 2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768,65536,131072,262144 ],
                 },
                 yaxis: {
                     label: '<center>Compression rate</center>',
                 }
             },
             legend: {
                 show: true,
                 location: 'nw',
                 placement: 'insideGrid'
             },
             highlighter: {
                 show: true,
                 sizeAdjust: 7.5
             },
             cursor: {
                 show: true,
                 zoom: true, 
                 showTooltip:false
             } 
         });
         $("#file").change(function(){
             console.log(data[this.value]);
             plot.series[0].data = data[this.value].lz77;
             plot.series[1].data = data[this.value].eac;
             plot.replot({resetAxes:true});
         });
         $('.button-reset').click(function() { plot.replot({resetAxes:true}); });
     });
    </script>
  </body>
</html>