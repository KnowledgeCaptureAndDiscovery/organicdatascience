var WTInProperties = function(title, inprops, util, api) {
	this.title = title;
	this.inprops = inprops;
	this.util = util;
	this.api = api;
};

WTInProperties.prototype.display = function($item) {
	this.$item = $item;
	this.$item.empty();
	this.$item.unmask();
	this.generateTable();
	var me = this;
};

WTInProperties.prototype.notify = function() {
	this.display();
};

WTInProperties.prototype.generateTable = function() {
	var me = this;

	me.$table =  $('<div class="wt-table"></div>');
	me.appendHeadingRow();
	me.$item.append(me.$table);
	for(var i=0; i<this.inprops.length; i++) {
		var subject = this.inprops[i].subject;
		var property = this.inprops[i].property;
		me.appendRow(subject, property)
	}
};

WTInProperties.prototype.appendRow = function(subject, property) {
	var me = this;
	var $row = $('<div class="wt-row"></div>');
	me.$table.append($row);
	me.appendIconCell($row);
	me.appendTripleCell($row, subject, property, this.title);
	return $row;
};

WTInProperties.prototype.appendHeadingRow = function() {
	var me = this;
	var heading  = '<div class="heading">';
		heading += '  <span><b>Incoming Properties</b></span>'
		heading += '</div>';
	me.$item.append($(heading));
};

WTInProperties.prototype.appendIconCell = function($row) {
	$cell = $('<div class="wt-cell wt-icon"></div>');
	var iconhtml = '<i class="fa fa-arrow-right fa-lg"></i> ';
	$cell.html(iconhtml);
	$row.append($cell);
};

WTInProperties.prototype.appendTripleCell = function($row, s, p, o) {
	$cell = $('<div class="wt-cell wt-content"></div>');
    var wgScriptPath = mw.config.get('wgScriptPath');
    var propprefix = wgScriptPath + '/index.php/Property:';
    var itemprefix = wgScriptPath + '/index.php/';
    $sentity = $('<a href="' + itemprefix + s +'" class="lodlink">' + this.typeToLabel(s) + '</a>');
    $pentity = $('<a href="' + propprefix + p +'" class="lodlink">' + this.typeToLabel(p) + '</a>');
    $oentity = $('<a href="' + itemprefix + o +'" class="lodlink">' + this.typeToLabel(o) + '</a>');
	$icon1 = '<i class="wt-icon fa fa-angle-double-right fa-lg"></i>';
	$icon2 = '<i class="wt-icon fa fa-angle-double-right fa-lg"></i>';
	$cell.append($sentity);
	$cell.append($icon1);
	$cell.append($pentity);
	$cell.append($icon2);
	$cell.append($oentity);
	$row.append($cell);
};

WTInProperties.prototype.appendContentCell = function($row, name) {
	$cell = $('<div class="wt-cell wt-label"></div>');
    var wgScriptPath = mw.config.get('wgScriptPath');
    var uri = wgScriptPath + '/index.php/' + name
    $entity = $('<a href="' + uri + '" class="lodlink">' + this.typeToLabel(name) + '</a>');
	$cell.append($entity);
	$row.append($cell);
};

WTInProperties.prototype.typeToLabel = function(pname) {
	return pname.replace(/_/g, ' ');
};

