var WTTracker = function(username, baseurl, util) {
	WTTracker.util = util;
	WTTracker.username = username;
	WTTracker.url = baseurl+"/extensions/WorkflowTasks/includes/tracking/WTTrackAPI.php";
	WTTracker.hoverlog = [];
}

/** Components */
WTTracker.c = {
	explorer: 'explorer',
	context: 'context',
	subtasks: 'subtasks',
	alert: 'alert',
	timeline: 'timeline',
	metadata: 'metadata',
	resizer: 'resizer',
	person: 'person',
	page: 'page',
	facts: 'facts',
	answers: 'answers',
	docu: 'docu',
	persexpert: 'persexpert',
	perstasks: 'perstasks'
	
};

/** Subcomponents */
WTTracker.s = {
	all: 'all',
	my: 'my',
	type: 'type',
	progress: 'progress',
	start: 'start',
	target: 'target',
	owner: 'owner',
	participants: 'participants',
	expertise: 'expertise'	
};

/** ActionTypes */
WTTracker.t = {
	nav: 'nav',
	search: 'search',
	filter: 'filter',
	add: 'add',
	del: 'del',
	set: 'set',
	move: 'move',
	rename: 'rename',
	cut: 'cut',
	resize: 'resize',
	open: 'open',
	hover: 'hover'
};


WTTracker.track = function( data ) {
	if(__disable_tracking) {
		if(data.callback)
			data.callback();
		return;
	}
	var me = this;
	var requestd = {
		component: data.component,
		subcomponent: 'subcomponent' in data ? data.subcomponent: '',
		actiontype: data.actiontype,
		action: data.action,		
		taskId: 'taskId' in data ? data.taskId : '',
		value: 'value' in data ? data.value : '',
		user: WTTracker.username,
		screenWidth: window.screen.availWidth,
		screenHeight: window.screen.availHeight,
		windowWidth: $(window).width(),
		windowHeight: $(window).height(),
		call: 'track',
		domain: me.getDomain(),
	};			
	$.post( WTTracker.url, requestd, function( response ) {
		if(response.success)  
			console.log("action has been logged: "+ JSON.stringify(requestd));
//		else
//			console.log("action could not be logged: "+ JSON.stringify(requestd));
		if(data.callback)
			data.callback();
	});
};

WTTracker.trackHover = function($obj, component) {
	if(__disable_tracking)
		return;
	$obj.mouseenter(function(){
		if(WTTracker.hoverlog.indexOf(component) == -1) {
			WTTracker.track({
				component: component,			
				actiontype: WTTracker.t.hover,
				action: 'hover '+component		
			});
		}
		WTTracker.hoverlog.push(component)
	});
	
};

WTTracker.getDomain = function() {
	var path = document.location.pathname.split('/');
	return document.location.host+'/'+path[1];	
};




