function WTExplorerEvents(){};

WTExplorerEvents.attachOpenTaskEvent = function($tree, baseurl, track) {
	$tree.on('activate_node.jstree', function (e, data) {
		if(data.node && data.node.id != 'AddNewSubTask'){
			track(data.node.id, baseurl);
		}else{
			$line = $tree.find('.addSubtask');			
			$line.toggle();
			$line.find('input').val('');
			if($line.is( ":hidden" ))
				$tree.jstree('deselect_all');
			else
				$line.find('input').focus();
		}		
	});


};

WTExplorerEvents.attachAddTaskEvent = function($tree, expapi, api, title, baseurl, isRoot, filter, track) {
	var $line = $tree.find('.addSubtask');
	var $input = $line.find('input');
	
	/** need to prevent jsTree default click event */
	$line.click(function(e){
		e.preventDefault();
		e.stopPropagation();
	});
	$input.keyup(function(e){
	    if(e.keyCode == 13){
	    	var val = $input.val();	
	    	if(val=='')return;
			if(expapi.taskExist(val)){
    			new WTDialog({
    				icon: baseurl+'/extensions/WorkflowTasks/images/task/warning.png',
    				title: 'Already exist',
    				content: 'Task <b>'+val+'</b> already exist!',
    				options: ['Ok'],
    				callback: function(){}
    			});
			}else{
				expapi.lockAll('Adding SubTask..');
				$tree.jstree('deselect_all');
				$line.hide();			
				var owner = isRoot && 'mytasks' in filter ? filter.mytasks : 'unknown';
				var expertise = isRoot && 'expertise' in filter ? filter.expertise : 'unknown';
				api.addTask(title, val, isRoot, owner, expertise, function(data){
		    		expapi.notifyAll(data.wttasks.update);
		    		$tree.unmask();
		    	});
				track(JSON.stringify({title:val, owner:owner, expertise:expertise}));
			}
	    }
	    var taskId = $input.val();
	    if(expapi.taskExist(taskId))	  
	    	$input.css('color','#ccc');
	    else
	    	$input.css('color','#0645ad');
	});
};


WTExplorerEvents.appendNewTaskNode = function(explorer, baseurl, expapi) {
	if(expapi.isLoggedIn())
		explorer.push({
				'id': 'AddNewSubTask',
				'text': '<span class="addSubtask" style="display:none;">'+
							'<input style="width:190px; margin:1px; font-size:99%; padding:2px; " type="text" placeholder="New Task">'+
						'</span>',
				'icon' : baseurl+"/extensions/WorkflowTasks/includes/core/api/WTIconAPI.php?icon=new"
		});	
	return explorer;
};

