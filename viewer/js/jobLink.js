$(document).ready(function(){
    $('button.jobLink').click(function(){
        var data = {
            cmd: $(this).data('cmd'),
            name: $(this).data('name'),
            container: $(this).data('container'),
            when: $(this).data('when'),
            priority: $(this).data('priority'),
            intArg: $(this).data('intArg'),
            strArg: $(this).data('strArg'),
        }
        $.post('/scheduler/schedule',data,function(result){
            $.bootstrapGrowl(result.text,{type:result.type,delay:5000,offset:{from:'top',amount: 60}});
        });
    });
});
