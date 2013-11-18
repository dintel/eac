$(document).ready(function(){
    $.getJSON('/job/distribution-graph-data',function(data){
        var s1 = data.r;
        var s2 = data.i;
        var ticks = data.names;
        plot = $.jqplot('jobsChart',[s1,s2],{
            title: 'Jobs queue distribution',
            stackSeries: true,
            seriesDefaults:{
                renderer:$.jqplot.BarRenderer,
                rendererOptions: {
                    // Put a 30 pixel margin between bars.
                    barMargin: 30,
                    // Highlight bars when mouse button pressed.
                    // Disables default highlighting on mouse over.
                    highlightMouseDown: true   
                },
                pointLabels: {show: true}
            },
            series:[
                {label:'Running'},
                {label:'Initial'}
            ],
            axes: {
                xaxis: {
                    renderer: $.jqplot.CategoryAxisRenderer,
                    ticks: ticks
                },
                yaxis: {
                    padMin: 0
                }
            },
            legend: {
                show: true,
                location: 'e',
                placement: 'outside'
            } 
        });
    });
});