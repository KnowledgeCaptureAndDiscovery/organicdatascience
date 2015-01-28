var WTTimeline = function(expapi, $timeline) {
	this.expapi = expapi;	
	this.$timeline = $timeline;
	this.color = {
		link: '#0645ad',
		gray: '#ddd',
		gray2: '#1d1d1b',
		red: 'red',
		blue_layout: '#a7d7f9',
		GREEN_LIGHT: '#b7db7b', 
		GREEN_MEDIUM: '#76b531',
		GREEN_DARK: '#448200',
		GREEN_DARKDARK: '#264801',
		GRAY_LIGHT: '#d9d9d9',
		GRAY_DARK: '#a6a6a6',
		GRAY_DARKDARK: '#848282',
		ORANGE_LIGHT: '#fac090',
		ORANGE_DARK: '#e46c0a',
		ORANGE_DARKDARK: '#a21418',
		YELLOW_LIGHT: '#fff9ae',
		YELLOW_MEDIUM: '#ffee75',
		YELLOW_DARK: '#ffd800',
		YELLOW_DARKDARK: '#ba9e01',
	};	
	this.dim = {
		height:{
			paddingtop: 15,
			line: 30,
			xaxis: 20
		},	
		width:{
			paddingleftDefault: 15,
			paddingleft:15,
			assov: 15,
			assoh: 15,
			label: 120,
			bar: 400,
			paddingright:10
		}
	};	
	this.expapi.subscribe(this);
	this.initPaper();
};

WTTimeline.prototype.display = function() {
	var me = this;
	me.paper.clear();
	me.initData(); 
	if(me.subtasks.length>0){
		me.calcLabelWidth();
		me.calcPaperPositions();
		me.drawTimeline();	
	}else{
		me.noSubtasksMsg();
	}	
};

WTTimeline.prototype.notify = function() {
	this.display();
};

WTTimeline.prototype.noSubtasksMsg = function() {
	var l = !this.ctx.parent ? '36px': '52px';
	$msg = $('<div class="msg">Has No Subtasks</div>');	
	$msg.css('padding-left', l);
	this.$timeline.html($msg);
};

/** Creates a new Raphael paper with size of parameters */
WTTimeline.prototype.initPaper = function() {
	var e = document.getElementById(this.$timeline.attr('id'));
	this.paper = new Raphael(e, "100%", "100%");
};

/** Initialized the explorer data*/
WTTimeline.prototype.initData = function( ) {
	var me = this;
	me.ctx = me.expapi.context();	
	me.subtasks = new Array();
	$.each(me.ctx.explorer, function(k, t) {
		var task = {};
		task.id = t.id;
		task.title = t.title;
		task.meta = t.facts.missingparam;
		task.warning = Object.keys(t.facts.warnings).length>0;
		if(task.meta){
			task.progress = t.facts.metaprogress/100;
		}else{
			task.start = t.facts.start;
			task.target = t.facts.target;
			task.type = t.facts.type;
			task.progress = t.facts.progress/100;
			task.overdue = t.facts.overdue;
		}		
		me.subtasks.push(task);
	});
};

/** Calculates the task label width depending on the tile */
WTTimeline.prototype.calcLabelWidth = function() {
	var me = this;
	var width = 0;
	$.each(me.subtasks, function(k, t) {
		var r = me.calcTile(t.title, me.dim.width.label);
		width = Math.max(width, r.width);
		t.titleLines = r.lines;
	});
	me.dim.width.label=width+10;
};

/** Calculates the logical paper reference positions */
WTTimeline.prototype.calcPaperPositions = function() {
	var me = this;	
	if(!me.ctx.parent)
		me.dim.width.paddingleft = me.dim.width.paddingleftDefault-15;
	me.dim.posx = {
		assov: me.dim.width.paddingleft + me.dim.width.assov,
		assoh: me.dim.width.paddingleft + me.dim.width.assov,
		label: me.dim.width.paddingleft + me.dim.width.assov + me.dim.width.assoh,
		bar: me.dim.width.paddingleft + me.dim.width.assov + me.dim.width.assoh + me.dim.width.label
	};
	me.dim.posy = {
		paddingtop: 0,
		line: me.dim.height.paddingtop,
		xaxis: me.dim.height.paddingtop + me.dim.height.line * me.subtasks.length
	};
	var width = me.dim.posx.bar + me.dim.width.bar;
	var height = me.dim.posy.xaxis + me.dim.height.xaxis;
	me.$timeline.height(height);
};

/** Main drawing method */
WTTimeline.prototype.drawTimeline = function() {
	var me = this;	
	var dateRange = me.calcDateRange(); 
	var posy = me.dim.height.paddingtop;	
	$.each(me.subtasks, function(k, subtask) {
		  var posyc = posy + me.dim.height.line/2;
		  me.drawName(subtask, posy, posyc);
		  me.drawBar(subtask, posy, posyc, dateRange);		  
		  posy+=me.dim.height.line;
	});	
	me.drawVerticalAsso();
	me.drawToday(dateRange);
//	me.drawXaxis(dateRange);
};

/** Calculates the date range of all the tasks */
WTTimeline.prototype.calcDateRange = function() {
	var me = this;
	var min = -1;
	var max = -1; 
	$.each(me.subtasks, function(k, subtask) {
		if(!subtask.meta){
			if(min==-1 && max==-1){
				min = subtask.start;
				max = subtask.target;
			}else{
				min = Math.min(min, subtask.start);
				max = Math.max(max, subtask.target);
			}			
		}
	});	
	var range = max-min;
	return {'min':min, 'max':max, 'range': range, 'factor': me.dim.width.bar/range};
}

/** Draws the vertical association line */
WTTimeline.prototype.drawVerticalAsso = function() {
	var me = this;
	var assoLineHeight = me.subtasks.length * me.dim.height.line + me.dim.height.paddingtop - me.dim.height.line/2;
	me.drawAssoLine(me.dim.posx.assov, 0, 0, assoLineHeight);
};

/** Draws the tasks name label and 
 *  the horizontal association line */
WTTimeline.prototype.drawName = function(subtask, posy, posyc) {  
   var me = this; 
   
   // text label 
   var yc = posyc;
   if(subtask.titleLines.length > 1)
	   yc-=8;		   
   var lineWidth = 0;
   $.each(subtask.titleLines, function(k, line) {
	   var t = me.paper.text(me.dim.posx.label, yc, line);
	   t.attr({'font-size': 11, 'text-anchor': 'start', 'fill': me.color.link});
	   t.mouseover(me.mouseoverCallback);
	   t.click(me.clickCallback);
	   t.data('taskid', subtask.id);
	   lineWidth = Math.max(lineWidth, t.getBBox().width);
	   yc+=12;
	   if(k==1) return false;
   });
   
   // horizontal asso line before text    
   var width = me.dim.posx.label-me.dim.posx.assoh;
   me.drawAssoLine(me.dim.posx.assoh, posyc, width, 0);
   
   // horizontal asso line after text    
   var cx = me.dim.posx.label+lineWidth;
   var width = me.dim.posx.bar-cx;
   me.drawAssoLine(cx, posyc, width, 0);
}

/** Draws a association line */
WTTimeline.prototype.drawAssoLine = function(x, y, xdirection, ydirection) {
	var asso = this.paper.path("M "+x+" "+y+" l "+xdirection+" "+ydirection);
	asso.attr({'stroke': 'black', 'stroke-width': 0.5, "stroke-dasharray":". "});
};

/** Draws a association line */
WTTimeline.prototype.drawLifeLine = function(x, y, xdirection) {
	if(xdirection<0){
		xdirection*=-1;
		x-=xdirection
	}
	var x = this.paper.rect(x, y, xdirection, 1);
    x.attr({'fill': '#eeeeee', 'stroke-width': 0});
};


/** Draws the taks bars green, gray and the percentage */
WTTimeline.prototype.drawBar = function(subtask, posy, posyc, dateRange) {
   var me = this;   
   var barHeight = me.dim.height.line*.75;
   posy += me.dim.height.line*.25/2;    
   if(subtask.meta)
	   me.drawMetaBar(subtask, posy, posyc, barHeight);	   
   else
	   me.drawNotMetaBar(subtask, posy, posyc, dateRange, barHeight);     
};

/** Draws the not meta bar */
WTTimeline.prototype.drawNotMetaBar = function(subtask, posy, posyc, dateRange, barHeight) {
	var me = this;
	var diff = subtask.target-subtask.start;
	var greenBarx = me.dim.posx.bar + (subtask.start-dateRange.min)*dateRange.factor; 
	var greenBarWidth = diff*subtask.progress*dateRange.factor;
	var grayBarx = greenBarx + greenBarWidth;
	var grayBarWidth = diff*(1-subtask.progress)*dateRange.factor;
	   
	me.drawLifeLine(me.dim.posx.bar, posyc, me.dim.width.bar);
	   
	// draw green background
	var t = me.paper.rect(greenBarx, posy, greenBarWidth, barHeight);
	t.attr({'fill': me.doneColor(subtask), 'stroke-width': 0});
	t.mouseover(me.mouseoverCallback);
	t.click(me.clickCallback);
	t.data('taskid',subtask.id);
	   
	// draw gray part
	var t = me.paper.rect(grayBarx, posy, grayBarWidth, barHeight);
	t.attr({'fill': me.leftColor(subtask), 'stroke-width': 0});
	t.mouseover(me.mouseoverCallback);
	t.click(me.clickCallback);
	t.data('taskid',subtask.id);
	   
	// progress text label
	var labelx = greenBarx + (greenBarWidth+grayBarWidth)/2;
	var progress = Math.round(subtask.progress*100)+"%";
	var t = me.paper.text(labelx, posyc, progress);
	t.attr({'font-size': 11, 'font-weight':'bold', 'fill': me.textColor(subtask)});
	t.mouseover(me.mouseoverCallback);
	t.click(me.clickCallback);
	t.data('taskid',subtask.id);
};

/** Draws the meta bar */
WTTimeline.prototype.drawMetaBar = function(subtask, posy, posyc, barHeight) {
	var me = this;
	var paddingleft = 20;
	var metabarwidth = 30;	  
   	var doneWidth = metabarwidth*subtask.progress;
   	var leftWidth = metabarwidth-doneWidth;
   	var xstart = me.dim.posx.bar+paddingleft;
   	var xdone = xstart + doneWidth;
   	   
   	me.drawLifeLine(xstart, posyc, -paddingleft);
   	var assowidth = me.dim.width.bar-(paddingleft+metabarwidth);
   	me.drawLifeLine(xstart+metabarwidth, posyc, assowidth);
   
   	if(subtask.progress<0.01){
	   	var meta = me.paper.path("M "+xstart+" "+posy+" l "+metabarwidth+" 0 l 0 "+barHeight+" l -"+metabarwidth+" 0 z");
	   	meta.attr({'stroke': me.leftColor(subtask), 'stroke-width': 1});
  	}else{
	   	var metaLeft = me.paper.path("M "+xdone+" "+posy+" l -"+doneWidth+" 0 l 0 "+barHeight+" l "+doneWidth+" 0");
	   	metaLeft.attr({'stroke': me.doneColor(subtask), 'stroke-width': 1});
	   
	   	var metaDone = me.paper.path("M "+xdone+" "+posy+" l "+leftWidth+" 0 l 0 "+barHeight+" l -"+leftWidth+" 0");
	   	metaDone.attr({'stroke': me.leftColor(subtask), 'stroke-width': 1});
   	}
   	var link = me.paper.rect(xstart, posy, metabarwidth, barHeight);
   	link.attr({'stroke-width': 0, 'fill': 'red', 'fill-opacity': '0', 'opacity': '0'});
   	link.mouseover(me.mouseoverCallback);
   	link.click(me.clickCallback);
   	link.data('taskid',subtask.id);
  
   	var labelx = xstart + metabarwidth/2;
   	var progress = Math.round(subtask.progress*100)+"%";
   	var t = me.paper.text(labelx, posyc, progress);
	t.attr({'font-size': 11, 'font-weight':'bold', 'fill': me.textColor(subtask)});
 	t.mouseover(me.mouseoverCallback);
   	t.click(me.clickCallback);
   	t.data('taskid',subtask.id);
};

/** Draws the xaxis arrow */
WTTimeline.prototype.drawXaxis = function(dateRange) {
	var me = this;
	
	// xaxis line
	var width = me.dim.width.bar+me.dim.width.paddingright;
    var x = me.paper.rect(me.dim.posx.bar, me.dim.posy.xaxis+5, width, 1);
    x.attr({'fill': me.color.blue_layout, 'stroke-width': 0});
    
    // xaxis line arrow
    var arrowx = me.dim.posx.bar + me.dim.width.bar-7 + me.dim.width.paddingright;
    var arrowy = me.dim.posy.xaxis;
    var arrow = me.paper.path("M "+arrowx+" "+arrowy+" l 7 5 l -7 5");
    arrow.attr({'stroke': me.color.blue_layout, 'stroke-width': 1});
};

/** Callback handler - mouse over on task element */
WTTimeline.prototype.mouseoverCallback = function() {
	this.attr({'cursor':'pointer'});
};

/** Callback handler - open timeline for subtask */
WTTimeline.prototype.clickCallback = function() {
	var taskId = this.data('taskid');
    console.log("open timeline for taskId: "+taskId);
    WTTracker.track({
        component: WTTracker.c.timeline,
        actiontype: WTTracker.t.nav,
        action: 'open task',
        taskId: taskId,
        callback: function(){
        	var location = document.location.href;
        	location = location.substr(0,location.lastIndexOf('/')+1);
        	document.location.href = location+taskId;	
        }
    }); 	
};

/** Draw the todys marker */
WTTimeline.prototype.drawToday = function(dateRange) {
	var me = this;
	var today = new Date().getTime()/1000;
	if(dateRange.min <= today && today <= dateRange.max){		
		var assoLineHeight = me.subtasks.length*me.dim.height.line+5;
		var posx = me.dim.posx.bar + (today-dateRange.min)*dateRange.factor;
		var asso = me.paper.path("M "+posx+" "+me.dim.height.paddingtop+" l 0 "+assoLineHeight);
		asso.attr({'stroke': me.color.red, 'stroke-width': 1});
	    // today text label
	    var t = me.paper.text(posx, me.dim.posy.line-5, 'TODAY');
	    t.attr({'font-size': 8, 'font-weight':'bold', 'fill': me.color.gray});
	}
};

/** Translates a task type into a done bar color */
WTTimeline.prototype.doneColor = function(task) {
	if(task.warning)
		return this.color.YELLOW_DARK;	
	else
		if(task.meta)
			return this.color.GREEN_MEDIUM;
		else		
			if(task.overdue)
				return this.color.ORANGE_DARK;
			else
				switch(task.type){
					case 'low': 	return this.color.GREEN_DARK;
					case 'medium': 	return this.color.GREEN_MEDIUM;
					case 'heigh': 	return this.color.GREEN_LIGHT;
					default: '';
				}
};

/** Translates a task type into a left bar color */
WTTimeline.prototype.leftColor = function(task) {
	if(task.warning)
		return this.color.YELLOW_LIGHT;	
	else
		if(task.meta)
			return this.color.GRAY_LIGHT;
		else
			if(task.overdue)
				return this.color.ORANGE_LIGHT;
			else
				return this.color.GRAY_LIGHT;
};

/** Translates a task type into a text label color */
WTTimeline.prototype.textColor = function(task) {
	if(task.warning)
		return this.color.YELLOW_DARKDARK;
	else
		if(task.meta)
			return this.color.GREEN_MEDIUM;
		else
			if(task.overdue)
				return this.color.ORANGE_DARKDARK;
			else
				switch(task.type){
					case 'low': 	return this.color.GREEN_DARKDARK;
					case 'medium': 	return this.color.GREEN_DARKDARK;
					case 'heigh': 	return this.color.GREEN_DARKDARK;
					default: '';
				}	
};

/** Caculates the lines of title */
WTTimeline.prototype.calcTile = function(text, maxwidth) {
	var me = this;	
	var gwidth = me.textWidth(text);
	var lines = [];
	
	if(gwidth > maxwidth){
		/** Split into 2 lines */
		var space = me.textWidth("-");
		var words = [];	
		$.each(text.split(" "), function(k, word) {
			words.push({
				text: word,
				width: me.textWidth(word)
			});
		});
		
		var half = gwidth/2;
		var gwidth = 0;
		lines[0] = '';
		lines[1] = '';
		if(half > maxwidth){
			/** More than 2 full lines */
			var widx = 0;
			var width = 0;			
			for(var i = 0; i<2;){
				if(width+words[widx].width < maxwidth){					
					lines[i]+=words[widx].text+" ";
					width+=words[widx++].width+space;
				}else{
					if(i==1){
						lines[i]+="...";
						width += me.textWidth("...");
					}
					i++;
					gwidth = Math.max(gwidth, width);
					width = 0;
				}
			}
		}else{
			/** Less than 2 full lines */
			var first = true;
			$.each(words, function(k, word) {
				if(first && gwidth<half){
					lines[0]+=word.text+" ";
					gwidth+=word.width+space;
					if(width>=half)
						first = false;
				}else{ 
					lines[1]+=word.text+" ";
				}
			});
		}		
	}else{
		/** Fits into one line */
		lines.push(text);
	}	
	return {'lines': lines, 'width': gwidth};
};

/** Calculates a text with based on raphael */
WTTimeline.prototype.textWidth = function(text) {
	var t = this.paper.text(0, 0).attr({'font-size': 11, 'text-anchor': 'start'});
	t.attr("text", text);
	var width = t.getBBox().width
	t.remove();
	return width;
};
