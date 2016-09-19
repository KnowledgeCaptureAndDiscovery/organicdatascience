var WTPerson = function(title, details, util, api ) {
	this.title = title;
	this.details = details;
	this.util = util;
	this.api = api;
	this.userid = null;
};

WTPerson.prototype.display = function( item ) {
	var me = this;
	var wgScriptPath = mw.config.get('wgScriptPath');
	item.data('data', me.details);

	this.userid = null;
	if(allwtfacts['Has User ID']) 
		this.userid = allwtfacts['Has User ID']['values'][0]['val'];

	var topSection = $('<div class="top-section"></div>');
	if(!wtuid)
		item.empty();
	if(this.userid && wtuid != this.userid) {
		item.find(".editable").removeClass('editable');
		wtrights['edit-page-metadata'] = false;
	}
	item.prepend(topSection);

	var default_pic_url = wgScriptPath+"/extensions/WorkflowTasks/images/default-user.png";
	var pic_url = null;
	if(wtuid && allwtfacts['Picture URL'])
		pic_url = allwtfacts['Picture URL']['values'][0]['val'];

	var picturediv = $("<div class='picture-section'></div>");
		picturediv.addClass('editable');

	var imgsrc = pic_url ? pic_url : default_pic_url;
	picturediv.append("<img src='"+imgsrc+"'></img>");
	topSection.append(picturediv);

	if(wtrights['edit-page-metadata']) {
		var editpic = $("<div class='picture-edit'><i class='fa fa-edit fa-3x'></i></div>");
		picturediv.append(editpic);
		editpic.click(function() {
			var url = prompt("Please enter Picture URL", pic_url ? pic_url : "");
			if(url != null) {
				topSection.mask('Setting Picture');
				me.api.replaceFact( me.title, 'Picture URL', url, pic_url, function(response) {
					if(!response || !response.wtfacts) return; 
					if(response.wtfacts.result == 'Success') {
						topSection.unmask();
						pic_url = url;
						picturediv.find('img').attr('src', url ? url : default_pic_url);
					}
				});
			}
		});
	}

	var summarydiv = $("<div class='summary-section'></div>");
	this.showContributions(summarydiv);
	this.showWorkingGroups(summarydiv);
	this.showPrivileges(summarydiv);

	if(!wtuid) {
		$('#ca-viewsource').css('display', 'none');
		$('#ca-history').css('display', 'none');
	}
	topSection.append(summarydiv);
};

WTPerson.prototype.showContributions = function(summarydiv) {
	var wgScriptPath = mw.config.get('wgScriptPath');
	var contribs = this.details.WTPerson.contributions;
	if(!contribs.length) {
		var wrapper = $('<div style="padding:5px"></div>');
		wrapper.append('No contributions by this person to the wiki.');
		wrapper.append('If this person has an account on the wiki, set the "Has User ID" property to their Wiki User ID.'); 
		summarydiv.append(wrapper);
		return;
	}
	else {
		var thisdiv = $('<div></div>');
		thisdiv.append("<div class='summary-header'>Contributions:</div>");
		var counts = {};
		var pages = {};
		var keytexts = {
			'Page:Create:Dataset:LiPD': 'Imported %d LiPD',
			'Page:Create:Dataset:User': 'Created %d datasets',
			'Page:Edit:Dataset:User': 'Edited %d datasets',
			'Ontology:Create:*:User': 'Created %d terms',
			'Ontology:Edit:*:User': 'Edited %d terms',
			'Page:Create:*:User': 'Created %d pages',
			'Page:Edit:*:User': 'Edited %d pages'
		};
		for(var i=0; i<contribs.length; i++) {
			var contrib = contribs[i];
			var key = null;
			if(contrib[2] == "Create" && contrib[1] == "Dataset_"+wgCore) 
				key = 'Page:Create:Dataset:'+contrib[4];
			else if(contrib[2] == "Edit" && contrib[1] == "Dataset_"+wgCore) 
				key = 'Page:Edit:Dataset:'+contrib[4];
			else if(contrib[4] == "Bootstrap")
				key = null; //contrib[4] + " items";
			else if(contrib[4] == "User" && contrib[0].match(/^(Category|Property):/))
				key = 'Ontology:'+contrib[2]+':*:'+contrib[4];
			else if(contrib[4] == "User" && !contrib[0].match(/^.+:/))
				key = 'Page:'+contrib[2]+':*:'+contrib[4];
			if(key) {
				if(!counts[key]) counts[key] = 0;
				counts[key]++;
				if(!pages[key]) pages[key] = [];
				pages[key].push(contrib[0]);
			}
		}
		for(var key in counts) {
			if(keytexts[key]) {
				var text = keytexts[key].replace("%d", "<b>"+counts[key]+"</b>");
				var summaryitem = $("<div class='summary-item'></div>");
				var summaryhref = $("<a>"+text+"</a>");
				summaryhref.data('key', key);
				summaryhref.data('keytext', text);
				summaryitem.append(summaryhref);
				thisdiv.append(summaryitem);
				summaryhref.click(function(e) {
					var listdiv = $('<div></div>');
					listdiv.append($('<span class="ui-helper-hidden-accessible"><input type="checkbox"/></span>'));

					var listul = $("<ul class='summary-list'></ul>");
					listdiv.append(listul);
					var listpages = pages[$(this).data('key')];
					var keytext = $(this).data('keytext');
					for(var j=0; j<listpages.length; j++) {
						listli = $("<li></li>");
						listli.append("<a href='" + wgScriptPath + "/index.php/" + listpages[j]+"'>" + listpages[j]+"</a>");
						listul.append(listli);
					}
					$(listdiv).dialog({
						title: keytext,
						width: 320,
						height: 280,
						modal: true
					});
				});
			}
		}
		summarydiv.append(thisdiv);
	}
};

WTPerson.prototype.showWorkingGroups = function(summarydiv) {
	var me = this;
	var groups = [];
	var hasGroups = false;
	if(allwtfacts['Subscribes To']) {
		groupfacts = allwtfacts['Subscribes To']['values'];
		for(var i=0; i<groupfacts.length; i++) {
			groups[groupfacts[i]['val']] = true;
			hasGroups = true;
		}
	}
	if(!hasGroups && (!wtuid || this.userid != wtuid))
		return;

	var thisdiv = $('<div></div>');
	var wgScriptPath = mw.config.get('wgScriptPath');
	thisdiv.append("<div class='summary-header'>Working Groups:</div>");
	
	var selectBox = $('<select multiple data-placeholder="Choose a Working Group" class="wg-select" style="width:100%"></select>');
	if(!wtuid || wtuid != this.userid) {
		selectBox.attr('disabled', 'disabled');
	}
	var groupdata = [];
	var allgroups = this.details.WTPerson.allgroups;
	if(!allgroups) allgroups = [];
	for(var i=0; i<allgroups.length; i++) {
		var group = allgroups[i];
		var groupname = group.replace(/^.*:/, "").replace(/_/g, " ");
		var grouplink = wgScriptPath + "/index.php/" + group;
		groupdata.push({id: group, text: groupname, link: grouplink});
		if(groups[group]) {
			var option = $('<option value="'+group+'" selected>'+groupname+'</option>');
			selectBox.append(option);
		}
	}
	selectBox.on('select2:select', function(evt) {
		var data = evt.params.data;
		thisdiv.mask(lpMsg('Adding Working Groups..'));
		me.api.addFact( me.title, 'Subscribes To', data['id'], function(response) {
			thisdiv.unmask();
			if(!response || !response.wtfacts) return; 
			if(response.wtfacts.result == 'Success') {
				// Do something ?
			}
		});
	});
	selectBox.on('select2:unselect', function(evt) {
		var data = evt.params.data;
		thisdiv.mask(lpMsg('Removing Working Group..'));
		me.api.removeFact( me.title, 'Subscribes To', data['id'], function(response) {
			thisdiv.unmask();
			if(!response || !response.wtfacts) return; 
			if(response.wtfacts.result == 'Success') {
				// Do something ?
			}
		});
	});

	thisdiv.append(selectBox);
	thisdiv.ready(function() {
		selectBox.select2({data: groupdata});
	});
	summarydiv.append(thisdiv);
};

WTPerson.prototype.showPrivileges = function(summarydiv) {
	var privileges = this.details.WTPerson.wikiuser.groups;
	var wgScriptPath = mw.config.get('wgScriptPath');
	if(!this.userid || !privileges.length)
		return;
	var thisdiv = $('<div></div>');
	thisdiv.append("<div class='summary-header'>Wiki Privileges:</div>");
	for(var i=0; i<privileges.length; i++) {
		var priv = privileges[i];
		var summaryitem = $("<div class='summary-item'></div>");
		var privlink = wgScriptPath + "/index.php/Special:ListUsers?group=" + priv;
		var summaryhref = $("<a href='"+privlink+"'>"+priv+"</a>");
		summaryitem.append(summaryhref);
		thisdiv.append(summaryitem);
	}
	summarydiv.append(thisdiv);
};
