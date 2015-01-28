var WTTaskAlert = function(expapi, baseurl, $menue) {
	this.baseurl = baseurl;
	this.expapi = expapi;
	this.expapi.subscribe(this);
	this.injectHtml($menue);
};

WTTaskAlert.prototype.display = function() {
	var me = this;		
	if(me.expapi.isLoggedIn()){
		me.alerts = me.expapi.alertList();
		me.initAlertBell();
		me.initAlertBox();		
	}   
};

WTTaskAlert.prototype.notify = function() {
	var me = this;
	if(me.expapi.isLoggedIn()){
		me.$bell.tooltipster('destroy');
		me.display();
	}
};

WTTaskAlert.prototype.injectHtml = function( $menue ) {
	this.$item = $('<li id="pt-alert" style="margin-top:0px; line-height:20px"></li>');
	$('#p-personal ul').prepend(this.$item);
}

WTTaskAlert.prototype.initAlertBox = function() {
	var me = this;			
	me.$bell.tooltipster({
        content: me.generateBoxContent(),
        theme: 'tooltipster-custom',
        interactive: true,
        functionBefore: function(origin, continueTooltip) { 
        	WTTracker.track({
        		component: WTTracker.c.alert,
        		actiontype: WTTracker.t.nav,
        		action: 'open alertbox'
        	});
        	continueTooltip(); 
        }
   });	   
};

WTTaskAlert.prototype.generateBoxContent = function() {
	var me = this;
	$headline = $('<div class="headline">Your Overdue Tasks</div>');	
	$box = $('<div id="pt-alertbox"></div>');	
	$box.append($headline);	
	if(me.alerts.length>0){
		$tasks = $('<div class="tasks"></div>');
		$tasks.css('max-height', $(window).height()-100);
		$box.append($tasks);
		$.each(me.alerts, function(k, task) {	
			var since = moment.unix(task.facts.target).fromNow();
			var $task = $('<div class="task"></div>');
			$task.css('background-image', "url('"+task.icon+"')");			
			$task.append('<div class="title">'+task.title+'</div>');
			$task.append('<div class="since">'+since+'</div>');
			$task.on('click', {taskId: task.id}, me.openTask);
			$tasks.append($task);
		});		
	}else{
		$box.append('<div class="noalert">You have no <br/>overdue tasks!</div>');
	}
	return $box;
};

WTTaskAlert.prototype.openTask = function(e) {
	WTTracker.track({
		component: WTTracker.c.alert,
		actiontype: WTTracker.t.nav,
		action: 'open task',
		taskId: e.data.taskId, 
		callback: function(){
			var location = document.location.href;
			location = location.substr(0,location.lastIndexOf('/')+1);
			document.location.href = location+e.data.taskId;	
		}		
	});
};

WTTaskAlert.prototype.initAlertBell = function() {
	var me = this;
	var hasAlert = me.alerts.length>0;
	$count = $('<span class="count">'+me.alerts.length+'</span>');
	$bell = $('<span style="line-height:20px;"><span>');
	$bell.append($count);
	if(hasAlert){
		$bell.css('padding-left','10px');
	}else{
		$bell.css('padding-left','20px');
		$count.hide();
	}	
	$bell.css('background-image', "url('"+me.baseurl+"/extensions/WorkflowTasks/includes/core/api/WTIconAPI.php?icon=alert&alert="+hasAlert+"')");
	me.$item.html($bell);
	me.$bell = $bell;
};
