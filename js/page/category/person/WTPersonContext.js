var WTPersonContext = function(baseurl) {
	this.baseurl = baseurl;
};

WTPersonContext.prototype.display = function( item ) {
	var me = this;
	$icon = $('<div>&nbsp;</div>');
	$icon.css('display', 'inline-block');
	$icon.css('width', '27px');
	$icon.css('background-image', 'url('+me.iconUrl()+')');
	item.prepend($icon);
};


WTPersonContext.prototype.iconUrl = function() {
	return this.baseurl+'/extensions/WorkflowTasks/includes/core/api/WTIconAPI.php?icon=person';
};