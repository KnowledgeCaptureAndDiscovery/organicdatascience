var WTPersonTasks = function(title, expapi, baseurl) {
	this.title = title;
	this.expapi = expapi;
	this.baseurl = baseurl;
	this.tasks = expapi.personsTasks(title);
};

WTPersonTasks.prototype.display = function( item ) {
	var me = this;
	me.$item = item;
	me.displayTasks(me.tasks.incurrent, 'incurrent');
	me.displayTasks(me.tasks.overdue, 'overdue');
	me.displayTasks(me.tasks.infuture, 'infuture');	
	me.displayTasks(me.tasks.inpast, 'inpast');
};

WTPersonTasks.prototype.displayTasks = function(tasks, category ) {
	var me = this;	
	$cat = $('<div class="category"></div>');	
	$label = $('<div class="wt-label"></div>')
	$text = $('<span></span>').text(me.label(category));
	$counter = $('<span class="counter"></span>');
	$counter.text(Object.keys(tasks).length);	
	$label.append($text);
	$label.append($counter);	
	$cat.append($label);
	
	$.each(tasks, function(k, v){
		$task = $('<div class="task"></div>');
		$task.attr('taskId', v.id);
		$task.css('background-image', 'url('+v.icon+')');		
		$task.click(function(){me.openTask(v.id)});
		$task.on( "mouseenter", function(){
			var taskId = $(this).attr('taskId');
			var e = me.taskById(taskId).facts.expertise;
			me.highlightExpertise(e);
		});
		$task.on( "mouseleave", function(){
			me.unhighlightExpertise();
		});		
		$title = $('<div class="title"></div>').append(v.title).append(me.ownerIcon(v.isOwner));
		$since = $('<div class="since"></div>').append(me.description(category, v));
		$task.append($title).append($since);
		$cat.append($task);
	});	
	me.$item.append($cat);	
};

WTPersonTasks.prototype.ownerIcon = function( isOwner ) {
	var url = this.baseurl+'/extensions/WorkflowTasks/includes/core/api/WTIconAPI.php?icon=personowner&isowner='+isOwner;
	$img = $('<img width="25" height="15" src="'+url+'"/>');
	$img.css('padding-left', isOwner ? '1px' : '4px');
	return $img;
};

WTPersonTasks.prototype.label = function( category ) {
	switch(category){
		case('infuture'): 	return 'Future Tasks';
		case('incurrent'): 	return 'Current Tasks';
		case('inpast'): 	return 'Completed Tasks';
		case('overdue'): 	return 'Overdue Tasks';
	}
};

WTPersonTasks.prototype.description = function( category, task) {
	var u = 'unknown';
	switch(category){
		case('infuture'): 	
			if(task.facts.start == u)
				return 'Start not defined yet!';
			return 'Starts '+moment.unix(task.facts.start).fromNow();
		case('incurrent'): 	
			if(task.facts.target == u)
				return 'Target not defined yet!';
			return 'Will be completed '+moment.unix(task.facts.target).fromNow();
		case('inpast'): 	
			if(task.facts.target == u)
				return 'Target not defined yet!';
			return 'Completed '+moment.unix(task.facts.target).fromNow();
	}
};

WTPersonTasks.prototype.openTask = function( taskId ) {
	var me = this;
	WTTracker.track({
        component: WTTracker.c.perstasks,
        actiontype: WTTracker.t.nav,
        action: 'open task',
        taskId: taskId,
        callback: function(){
        	location.href  = taskId
        }
    });
};

WTPersonTasks.prototype.highlight = function( expertise ) {
	var me = this;
	expertise = expertise.toLowerCase();
	me.$item.find('.task').each(function(){		
		var taskId = $(this).attr('taskId');
		var task = me.taskById(taskId);
		if(task.facts.expertise.indexOf(expertise) == -1)
			$(this).fadeTo( "slow" , 0.2, null);
	});
};
WTPersonTasks.prototype.unhighlight = function( ) {
	this.$item.find('.task').each(function(){		
		$(this).stop(true, true).fadeTo("fast", 1, null);	
	});
};
WTPersonTasks.prototype.taskById = function(taskId){
	var me = this;
	if(taskId in me.tasks.infuture)
		return me.tasks.infuture[taskId];
	if(taskId in me.tasks.incurrent)
		return me.tasks.incurrent[taskId];
	if(taskId in me.tasks.inpast)
		return me.tasks.inpast[taskId];
	return null;
};

WTPersonTasks.prototype.setPersonExpertise = function(personExpertise) {
	this.personExpertise = personExpertise;
};
WTPersonTasks.prototype.highlightExpertise = function(expertise) {
	this.personExpertise.highlight(expertise);
};
WTPersonTasks.prototype.unhighlightExpertise = function() {
	this.personExpertise.unhighlight();
};
