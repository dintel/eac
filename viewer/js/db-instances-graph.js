$(document).ready(function(){
    $.getJSON('/db-instance/distribution-graph-data',function(data){
        var s1 = data.r;
        var ticks = data.names;
        plot = $.jqplot('dbInstancesChart',[s1],{
            title: 'DBs distribution on RDS instances',
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
                {label:'Provisioned'},
            ],
            axes: {
                xaxis: {
                    renderer: $.jqplot.CategoryAxisRenderer,
                    ticks: ticks,
                },
                yaxis: {
                    tickOptions: { 
                        formatString: '%d' 
                    },
                    min: 0,
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
    $.getJSON('/db-instance/usage-graph-data',function(data){
        var s1 = data.u;
        var ticks = data.names;
        plot = $.jqplot('usageChart',[s1],{
            title: 'Total usage',
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
            axes: {
                xaxis: {
                    renderer: $.jqplot.CategoryAxisRenderer,
                    ticks: ticks,
                },
                yaxis: {
                    tickOptions: { 
                        formatString: '%d %%' 
                    },
                    min: 0,
                    max: 100,
                    padMin: 0
                }
            },
            legend: {
                show: false,
                location: 'e',
                placement: 'outside'
            } 
        });
    });
});