var WTWorkflow = function(title, details, util, api ) {
	this.title = title;
	this.details = details;
	this.util = util;
	this.api = api;
};

WTWorkflow.prototype.appendLinkItem = function( list, link, name ) {
	if(!name) name = link;
	var cls = link.match(/^http/i) ? 'external' : '';
	var vlink = link.replace(/\s/g, '_');
	var vname = name.replace(/.+\//, '');
	list.append($('<li></li>').append($('<a class="'+cls+'" href="'+vlink+'"></a>').append(vname)));
};

WTWorkflow.prototype.appendImageItem = function( item, imguri ) {
	imguri = imguri.replace(/\s/g, '_');
	var vimage = $('<img class="wflowimage" src="'+imguri+'"></img>');
	item.append($('<a href="'+imguri+'"></a>').append(vimage));
};


WTWorkflow.prototype.getListItem = function( list, wflow ) {
	var wflow_li = $('<li></li>');

	var me = this;
	if(wtuid) {
		var delhref = $('<a class="lodlink"><i class="fa fa-times-circle fa-lg delbutton"></i></a>');
		delhref.click( function(e) {
			list.mask(lpMsg('Removing Workflow Link..'));
			me.api.removeWorkflow( me.title, wflow.url, function(resp) {
				list.unmask();
				if(!resp || !resp.wtfacts) return;
				if(resp.wtfacts.result == 'Success') {
					me.addwflow_link.css('display', '');
					wflow_li.remove();
				}
			});
		});
		wflow_li.append(delhref).append(' ');
	}
	
	var wfname = wflow.url.replace(/.+\//, '');
	//wfname = wfname.replace(/_/g, ' ');
	var wflink = wflow.url.replace(/\s/g,'_');
	wflow_li.append($('<a href="'+wflink+'"></a>').append(wfname));

	var dvars = $('<ul></ul>');
	$.each(wflow.datavariables, function(i, v) {
		var vname = v.replace(/.+\//, '');
		vname = vname.replace(wfname+'_', '');
		me.appendLinkItem(dvars, v, vname);
	});

	var pvars = $('<ul></ul>');
	$.each(wflow.paramvariables, function(i, v) {
		var vname = v.replace(/.+\//, '');
		vname = vname.replace(wfname+'_', '');
		me.appendLinkItem(pvars, v, vname);
	});

	var procs = $('<ul></ul>');
	$.each(wflow.processes, function(i, p) {
		var pname = p.replace(/.+\//, '');
		pname = pname.replace(wfname+'_', '');
		me.appendLinkItem(procs, p, pname);
	});

	var executions = $('<ul></ul>');
	$.each(wflow.executions, function(i, p) {
		me.appendLinkItem(executions, p);
	});

	var contrib = $('<ul></ul>');
	this.appendLinkItem(contrib, wflow.contributor);

	var crsys = $('<ul></ul>');
	this.appendLinkItem(crsys, wflow.createdin);

	var wingstemp = $('<ul></ul>');
	this.appendLinkItem(wingstemp, wflow.wingstemplate);

	wflow_li.append($('<br><hr><b>Processes</b>')).append(procs);
	wflow_li.append($('<hr><b>Data Variables</b>')).append(dvars);
	wflow_li.append($('<hr><b>Parameter Variables</b>')).append(pvars);
	wflow_li.append($('<hr><b>Workflow Executions</b>')).append(executions);
	wflow_li.append($('<hr><b>Contributor</b>')).append(contrib);
	wflow_li.append($('<hr><b>Workflow Created In</b>')).append(crsys);
	wflow_li.append($('<hr><b>Template File</b>')).append(wingstemp);

	if(wflow.templateimage) {
		var timage = $('<div></div>');
		this.appendImageItem(timage, wflow.templateimage);
		wflow_li.append($('<hr><b>Workflow Template Image</b>')).append(timage);
	}

	return wflow_li;
};

WTWorkflow.prototype.getList = function( item, data ) {
	var list = $('<ul></ul>');

	var me = this;

	var ival = $('<input style="width:60%" type="text" />');
	var igo = $('<a class="lodbutton">' + lpMsg('Go') + '</a>');
	var icancel = $('<a class="lodbutton">' + lpMsg('Cancel') + '</a>');

	var addwflow_li = $('<li></li>').append($('<div style="width:24px"></div>'));
	addwflow_li.append(ival).append(igo).append(icancel).hide();
	list.append(addwflow_li);

	icancel.click(function( e ) {
		ival.val('');
		ival.data('val','');
		addwflow_li.hide();
	});

	igo.click(function(e) { localAdd() });
	ival.keyup(function(e) {
		if(e.keyCode == 13) { localAdd(); }
	});

	function localAdd() {
		var val = ival.data('val') ? ival.data('val') : ival.val();
		addwflow_li.hide();
		if(!val) return; 
		ival.val('');
		ival.data('val','');

		item.mask(lpMsg('Adding Workflow Link.. Please wait..'));
		me.api.addWorkflow( me.title, val, function(response) {
			item.unmask();
			if(!response || !response.wtfacts || !response.wtfacts.newdetails) return; 
			if(response.wtfacts.result == 'Success') {
				var data = response.wtfacts.newdetails;
				var wflow_li = me.getListItem(item, data.Workflow);
				me.addwflow_link.css('display', 'none');
				list.append(wflow_li);
			}
		});
	}

	if(data && data.Workflow) {
		list.append(me.getListItem(item, data.Workflow));
	}

	item.data('list', list);
	return list;
};


WTWorkflow.prototype.display = function( item ) {
	var me = this;

	item.data('data', me.details);

	var list = me.getList( item, me.details );

	if(wtuid) {
		me.addwflow_link = $('<a class="x-small lodbutton">' + lpMsg('Add Pubby Link') + '</a>');
		me.addwflow_link.click(function( e ) {
			list.find('li:first').css('display', '');
		});
	}

	var header = $('<div class="heading"></div>').append($('<b>Workflow</b>'));
	item.append(header);

	var wrapper = $('<div style="padding:5px"></div>');
	var toolbar = $('<div></div>').append(me.addwflow_link);
	//toolbar.append(me.util.getHelpButton('add_wflow')));
	wrapper.append(toolbar);
	wrapper.append(list);
	item.append(wrapper);

	if(me.details.Workflow) {
		me.addwflow_link.css('display', 'none');
	}
};

