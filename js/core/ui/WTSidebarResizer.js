var WTSidebarResizer = function(sidebar) {
	this.sidebarWith = 270; // 176 orginal with, 132 orginal inner width 
	this.minWidth = 250;
	this.sidebar = sidebar;
};


WTSidebarResizer.prototype.display = function() {
	var me = this;
	
	var $btn = $('<div id="content-resize-btn">&#9668;<br/>&#9658;</div>');	
	$('#content').before($btn);
	
	me.initResizeElements();
				
	var mouseDownOnBtn = -1;
	
	$btn.mouseover(function(e, element){			
		$btn.css('cursor','e-resize');
	});
	
	$btn.mousedown(function(e) {
		mouseDownOnBtn = e.clientX;
	});
	
	var crrentlyResizing = false;
	$btn.mousemove(function(e) {		
		if(mouseDownOnBtn != -1){
			var moveX = e.clientX-mouseDownOnBtn;
			if(Math.abs(moveX) >5 && !crrentlyResizing){
				crrentlyResizing = true;
				//console.log('move: '+moveX);
				me.sidebarWidthIncrease(moveX);		
				mouseDownOnBtn = e.clientX;
				crrentlyResizing = false;
			}				
		}
		//mouseDownOnBtn = -1;
	});
	

//	$btn.mouseout(function() {
//		mouseDownOnBtn = -1;
//	});		
	$(window).mouseup(function(e) {
//		if(e.clientX == mouseDownOnBtn){
//			me.sidebarWith+=100;
//			me.setSiteBarWith();
//		}
		mouseDownOnBtn = -1;
	});
	me.$important.remove();	
	me.setSiteBarWith();
};


WTSidebarResizer.prototype.sidebarWidthIncrease = function(increase) {
	var me = this;
	me.sidebarWith += increase;	
	if(me.sidebarWith < me.minWidth)
		me.sidebarWith = me.minWidth;
	if(!me.timeout)
		me.timeout = setTimeout(function(){
			me.setSiteBarWith(); me.timeout = null;
			me.track(increase);
			me.timeout = null;
		},5);	
};

WTSidebarResizer.prototype.track = function(increase) {
	var me = this;
	if(!me.tincrease)
		me.tincrease = 0;
	me.tincrease += increase;
	if(!me.trackTimeout)
		me.trackTimeout = setTimeout(function(){
			WTTracker.track({
				component: WTTracker.c.resizer,
				actiontype: WTTracker.t.resize,
				action: 'resize'+(me.tincrease>0? '++':'--'),
				value: me.sidebarWith
			});
			me.tincrease = 0;
			me.trackTimeout = null;
		},500);	
};


WTSidebarResizer.prototype.setSiteBarWith = function() {
	var me = this;
	me.$btn.css('margin-left', me.sidebarWith-40+'px');
	me.$content.css('margin-left', me.sidebarWith+'px');
	me.$pnav.css('width', me.sidebarWith-(176-132)+25+'px');
	me.sidebar.resize(me.sidebarWith-19);
};

WTSidebarResizer.prototype.initResizeElements = function() {
	var me = this;
	me.$btn = $('#content-resize-btn');
	me.$content = $('#content, #left-navigation, #footer');
	me.$pnav = $('#p-navigation');
	me.$important = $('#main-tree-sidebar-style');
};