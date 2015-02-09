var WTSidebar = function(title, $sidebar, expapi, api, baseurl, menu) {
	this.title = title;
	this.menu = menu;
	this.baseurl = baseurl;
	this.$sidebar = $sidebar;
	this.api = api;
	this.expapi = expapi;
	this.expapi.subscribe(this);
	this.taskWidth = 251;
};

WTSidebar.prototype.display = function() {
	var me = this;	
	me.activeTab = 'all';
	me.view = 'hierachie'
	me.filter = {};
	me.initStyle();
	me.initNavigationTabs();
	me.initFilters();
	me.initMessage();
	me.initExplorerDiv();
	me.initExplorer();	
};

WTSidebar.prototype.notify = function() {
	this.display();
};

WTSidebar.prototype.initNavigationTabs = function() {
	var me = this;
	var $navTabs = $('<div class="filtertabs"></div>');
	me.$sidebar.append($navTabs);
	var $navTabsRow = $('<div class="wt-row"></div>');
	$navTabs.append($navTabsRow);
	
	var $allTasks = $('<div class="wt-cell all first selected">All Tasks</div>');	
	$navTabsRow.append($allTasks);
	$allTasks.click(function(){
		me.switchTab($allTasks, $myTasks);
		me.activeTab = 'all';
		me.initExplorer();
		WTTracker.track({
			component: WTTracker.c.explorer,
			subcomponent: WTTracker.s.all,
			actiontype: WTTracker.t.nav,
			action: 'open tab all'
		});		
	});
	
	if(me.expapi.isLoggedIn()){
		var count = me.expapi.myTaskCount();
		var $myTasks = $('<div class="wt-cell my unselected">My Tasks <span class="counter">'+count+'</span></div>');	
		$navTabsRow.append($myTasks);
		$myTasks.click(function(){
			me.switchTab($myTasks, $allTasks);
			me.activeTab = 'my';
			me.initExplorer();
			WTTracker.track({
				component: WTTracker.c.explorer,
				subcomponent: WTTracker.s.my,
				actiontype: WTTracker.t.nav,
				action: 'open tab my'
			});	
		});		
	}
	
	var $space = $('<div class="space">&nbsp;</div>');	
	$navTabsRow.append($space);
	
	var $hirachView = $('<div class="wt-cellsmall"></div>');
	$navTabsRow.append($hirachView);
	var $hirachViewIcon = $('<div class="wt-icon first selected"><div class="hierarchy">&nbsp;</div></div>');
	$hirachView.append($hirachViewIcon);
	$hirachViewIcon.click(function(){
		me.switchTab($hirachViewIcon, $listViewIcon);
		me.view = 'hierachie';
		me.initExplorer();
		WTTracker.track({
			component: WTTracker.c.explorer,
			subcomponent: me.activeTab,
			actiontype: WTTracker.t.nav,
			action: 'select hierachie view'
		});	
	});
	
	var $listView = $('<div class="wt-cellsmall"></div>');
	$navTabsRow.append($listView);
	var $listViewIcon = $('<div class="wt-icon unselected"><div class="list">&nbsp;</div></div>');
	$listView.append($listViewIcon);
	$listViewIcon.click(function(){
		me.switchTab($listViewIcon, $hirachViewIcon);
		me.view = 'list';
		me.initExplorer();
		WTTracker.track({
			component: WTTracker.c.explorer,
			subcomponent: me.activeTab,
			actiontype: WTTracker.t.nav,
			action: 'select list view'
		});		
	});	
	$space = $('<div class="space" style="width:8px;"></div>');	
	$navTabsRow.append($space);

};

WTSidebar.prototype.switchTab = function($selected, $unselect) {
	$selected.removeClass('unselected');
	$selected.addClass('selected');
	$unselect.removeClass('selected');
	$unselect.addClass('unselected');
};

WTSidebar.prototype.initFilters = function() {
	var me = this;
	var $filters = $('<div class="filters"></div>');
	me.$sidebar.append($filters);

	/** expertise filter */
	var $expertise = $('<select class="unselected" placeholder="search"><option>Expertise filter</option></select>');
	$.each(me.expapi.allExpertiseFilters(), function(expertise, count) {
		var label = expertise+' ('+count+')';
		$expertise.append('<option value="'+expertise+'">'+label+'</option>');
	});
	$expertise.change(function(){
		if($expertise.val() == 'Expertise filter'){
			delete me.filter.expertise;
			$expertise.addClass('unselected');
			WTTracker.track({
				component: WTTracker.c.explorer,
				subcomponent: me.activeTab,
				actiontype: WTTracker.t.filter,
				action: 'remove expertise filter'
			});		
		}else{
			me.filter.expertise = $expertise.val();		
			$expertise.removeClass('unselected');
			WTTracker.track({
				component: WTTracker.c.explorer,
				subcomponent: me.activeTab,
				actiontype: WTTracker.t.filter,
				action: 'apply expertise filter',
				value: me.filter.expertise
			});		
		}
		me.initExplorer();

	});
	$filters.append($expertise);
	me.$expertise = $expertise;
	
	/** text filter */
	var $text = $('<input type="text" placeholder="search"/>');
	$text.on("keyup", function(){
		if($text.val() == '')
			delete me.filter.text;
		else
			me.filter.text = $text.val();
		me.initExplorer();
		WTTracker.track({
			component: WTTracker.c.explorer,
			subcomponent: me.activeTab,
			actiontype: WTTracker.t.search,
			action: 'text search',
			value: $text.val()
		});	
	});
	$filters.append($text);
	me.$search = $text;
	//me.resize(me.taskWidth);
};

WTSidebar.prototype.initMessage = function() {	
	this.$message = $('<div class="msg">No Results</div>');
	this.$message.hide();
	this.$sidebar.append(this.$message);
};

WTSidebar.prototype.updateMessage = function(isEmpty) {	
	if(isEmpty)
		this.$message.hide();
	else
		this.$message.show();
};

WTSidebar.prototype.readFilter = function() {	
	if(this.activeTab == 'my')
		this.filter.mytasks = this.expapi.username();
	else
		delete this.filter.mytasks;
	return this.filter;
};

WTSidebar.prototype.initExplorerDiv = function() {	
	this.$sidebar.append('<div style="font-size:11px;" id="tasktree" class="explorertree"></div>');	
};

WTSidebar.prototype.initExplorer = function() {	
	var me = this;		
	var filter = me.readFilter();
		
	/** filter data */
	var data = me.expapi.applayFilter(filter, me.view);	
	
	/** Add node to root */
	var addTask = me.view == 'hierachie' && !('text' in filter);
	if(addTask)
		data = WTExplorerEvents.appendNewTaskNode(data, me.baseurl, me.expapi);
	me.updateMessage(data.length>0);	
	
	/** init explorer tree */
	$explorer = me.$sidebar.find('#tasktree');	
	$explorer.jstree("destroy");		
	
	$explorer.jstree({
		  "core" : {
			    "animation" : 0,
			    "check_callback" : true,
			    'data' : data			    
		  },
		  "plugins" : [ "wholerow", "crrm", "contextmenu"],
		  "contextmenu": { "items": me.menu.explorerOptions }
	});		
	if(wtcategories['Task'])
		$explorer.jstree(true).select_node(me.expapi.selectedTaskId());

	$explorer.on("before_open.jstree", function (e, data) {
		WTTracker.track({
			component: WTTracker.c.explorer,
			subcomponent: me.activeTab,
			actiontype: WTTracker.t.nav,
			action: 'expand task',
			taskId: data.node.id
		});
	});
	$explorer.on("close_node.jstree", function (e, data) {
		WTTracker.track({
			component: WTTracker.c.explorer,
			subcomponent: me.activeTab,
			actiontype: WTTracker.t.nav,
			action: 'close task',
			taskId: data.node.id
		});
	});	
	WTExplorerEvents.attachOpenTaskEvent($explorer, me.baseurl, function(taskId, baseurl){
		WTTracker.track({
			component: WTTracker.c.explorer,
			subcomponent: me.activeTab,
			actiontype: WTTracker.t.nav,
			action: 'open task',
			taskId: taskId,
			callback: function(){
				console.log(taskId);
				document.location.href = baseurl+'/index.php/'+taskId;
			}
		});	
	});
	
	if(addTask)
		WTExplorerEvents.attachAddTaskEvent($explorer, me.expapi, me.api, me.title, me.baseurl, true, filter, function(value){
	        WTTracker.track({
	            component: WTTracker.c.explorer,
	            actiontype: WTTracker.t.add,
	            action: 'add root task',
	            taskId: me.title,
	            value: value
	        });
		});
};

WTSidebar.prototype.lock = function(message) {
	var $exp = this.$sidebar.find('#tasktree');	
	$exp.mask(lpMsg(message));
};

WTSidebar.prototype.resize = function(width) {
	var me = this;
	me.setTaskWidth(width);
	width -= 12;
	var ew = width > 296 ? 148 : width/2;
	var sw = width-ew;
	me.$expertise.css('width', ew+'px');
	me.$search.css('width', sw+'px');
	me.$search.css('background-position', sw-20+'px 4px');	
};

WTSidebar.prototype.initStyle = function() {
	var me = this;
	me.$css = $('<style></style>');
	me.$sidebar.html(me.$css);	
};

WTSidebar.prototype.setTaskWidth = function(width) {
	this.taskWidth = width;
	this.$css.text('#tasktree .jstree-node{width:'+width+'px}');
};
