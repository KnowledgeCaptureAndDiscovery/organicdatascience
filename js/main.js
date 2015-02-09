var lpMsg = function(key) {
	return key;
};

$(function() {
    // Custom autocomplete instance.
    $.widget( "app.autocomplete", $.ui.autocomplete, {
        options: { highlightClass: "ui-state-highlight" },
        _renderItem: function( ul, item ) {
            var re = new RegExp( "(" + this.term + ")", "gi" ),
                cls = this.options.highlightClass,
                template = "<span class='" + cls + "'>$1</span>",
                label = item.label.replace( re, template ),
                $li = $( "<li/>" ).appendTo( ul );
            $( "<a/>" ).attr( "href", "#" )
                       .html( label )
                       .appendTo( $li );
            return $li;
        }
    });

	var wtapi = new WTAPI(wgPageName, wgScriptPath+'/api.php');
	var wtutil = new WTUtil(wgPageName, wtapi);

//<<<<<<< HEAD
	if(!__use_simple_tasks) {
		var wtexpapi = new WTExplorerAPI(allwtexplorer, wgScriptPath, wtapi);	
		new WTTracker(wtexpapi.userid(), wgScriptPath, wtutil);
		var wtcats = []
		for(var cat in wtcategories)
			wtcats.push(cat);
	
		WTTracker.track({
			component: WTTracker.c.page,
			actiontype: WTTracker.t.nav,
			action: 'open page',
			taskId: wgPageName,
			value: wtcats.join()
		});	
	
		var $menu = $('#p-personal ul');
		var wtta = new WTTaskAlert(wtexpapi, wgScriptPath, $menu).display();	
		var wtmenu = new WTExplorerMenu(wgPageName, wtapi, wtexpapi, wgScriptPath);
		
		var $sidebar = $('#main-tree-sidebar');
		WTTracker.trackHover($sidebar, WTTracker.c.explorer);
		if($sidebar) {
			var $pnav = $('#p-navigation');
			var wtside = new WTSidebar(wgPageName, $sidebar, wtexpapi, wtapi, wgScriptPath, wtmenu)
			wtside.display();	
			if($pnav.css('display')) {
				$pnav.prepend($sidebar.detach());
				new WTSidebarResizer(wtside).display();	
			}
			else {
				$sidebar.css('display', 'none');
				var $btna = $('<a href="#" title="Toggle Task Tree" accesskey="t">Task tree</a>');
				$btna.on('click', function() {
					var curdisp = $sidebar.css('display');
					$sidebar.css('display', (curdisp=='none' ? '': 'none'));
				});
				var $li = $('<li id="n-task-toggle"></li>').append($btna);
				var $btn = $('<ul class="nav navbar-nav"></ul>').append($li);
				$('#mw-navigation-collapse').append($btn);
				$('#mw-navigation').append($sidebar.detach());				
			}
		}			
	
		if(wtcategories["Task"]) {			
			var metadiv = $j("#main-taskmetadata");
			WTTracker.trackHover(metadiv, WTTracker.c.metadata);
			var wtmd = new WTTaskMetaData(wgPageName, wtexpapi, wtapi, wgScriptPath, metadiv);		
			wtmd.display();

			var treediv = $j("#main-tree");
			WTTracker.trackHover(treediv, WTTracker.c.subtasks);
			var wtsubs = new WTSubTasks(wgPageName, wtexpapi, wtutil, wtapi, wgScriptPath, treediv, wtmenu);
			wtsubs.display();
		
			WTTracker.trackHover($('#main-facts'), WTTracker.c.facts);
					
			var wtanswers = new WTAnswers(wgPageName, allwtdetails, wtutil, wtapi);
			var answersdiv = $j("#main-answers");
			WTTracker.trackHover(answersdiv, WTTracker.c.answers);
			wtanswers.display(answersdiv);
		
			var timelinediv = $("#main-timeline");	
			WTTracker.trackHover(timelinediv, WTTracker.c.timeline);
			var wtt = new WTTimeline(wtexpapi, timelinediv);			
			wtt.display();
		
			$('#jump-to-nav, #contentSub').remove();
			var headingdiv = $("#firstHeading");		
			var wtctx = new WTTaskContext(wtexpapi, headingdiv, treediv, timelinediv);
			wtctx.display();		
		}
	}

	if(wtcategories["Task"] && __use_simple_tasks) {
		var wtanswers = new WTAnswers(wgPageName, allwtdetails, wtutil, wtapi);
		var answersdiv = $("#main-answers");
		wtanswers.display(answersdiv);

		var wtsubs = new WTSimpleTasks(wgPageName, allwtdetails, wtutil, wtapi);
		var treediv = $("#main-simple-tree");
		wtsubs.display(treediv);
	}
	else if(wtcategories["Procedure"]) {
		$("#main-answers").css('display', 'none');
		var wtsubs = new WTSimpleTasks(wgPageName, allwtdetails, wtutil, wtapi);
		var treediv = $("#main-simple-tree");
		wtsubs.display(treediv);
	}
	else if(wtcategories["Answer"]) {
		var wttasks = new WTTasks(wgPageName, allwtdetails, wtutil, wtapi);
		var tasksdiv = $("#main-tasks");
		wttasks.display(tasksdiv);
	}
	else if(wtcategories["Workflow"]) {
		var wtworkflow = new WTWorkflow(wgPageName, allwtdetails, wtutil, wtapi);
		var wflowdiv = $("#main-workflow");
		wtworkflow.display(wflowdiv);
	}
	else if(wtcategories["ExecutedWorkflow"]) {
		var wtworkflow = new WTExecutedWorkflow(wgPageName, allwtdetails, wtutil, wtapi);
		var wflowdiv = $("#main-workflow");
		wtworkflow.display(wflowdiv);
	}
	else if(wtcategories["AutomaticallyProvidedData"]) {
		var wtdata = new WTData(wgPageName, allwtdetails, wtutil, wtapi);
		var datadiv = $("#main-data");
		wtdata.display(datadiv);
	}
	else if(wtcategories["UserDescribedData"]) {
		var wtdata = new WTUserDescribedData(wgPageName, allwtdetails, wtutil, wtapi);
		var datadiv = $("#main-data");
		wtdata.display(datadiv);
		var wtdatacols = new WTDataColumns(wgPageName, allwtfacts, wtutil, wtapi);
		wtdatacols.display(datadiv);
	}
	else if(wtcategories["UserProvidedData"]) {
		var wtdata = new WTUserProvidedData(wgPageName, allwtdetails, wtutil, wtapi);
		var datadiv = $("#main-data");
		wtdata.display(datadiv);
		var wtdatacols = new WTDataColumns(wgPageName, allwtfacts, wtutil, wtapi);
		wtdatacols.display(datadiv);
	}
	else if(wtcategories["Component"]) {
		var wtcomp = new WTComponent(wgPageName, allwtdetails, wtutil, wtapi);
		var compdiv = $("#main-comp");
		wtcomp.display(compdiv);
	}
	else if(wtcategories["Person"] || wgNamespaceNumber == 2) {
		var wtperson = new WTPerson(wgPageName, allwtdetails, wtutil, wtapi);
		var persondiv = $("#main-person");
		wtperson.display(persondiv);
		if(!usesimpletasks) {
			var wtpctx = new WTPersonContext(wgScriptPath);
			wtpctx.display($('#firstHeading'));	
				
			var wtpexp = new WTPersonExpertise(wgPageName, wtexpapi, wtapi, allwtdetails);
			var wtpersexpertdiv = $('#main-personexpertise');
			WTTracker.trackHover(wtpersexpertdiv, WTTracker.c.persexpert);
			wtpexp.display(wtpersexpertdiv);
		
			var wtptasks = new WTPersonTasks(wgPageName, wtexpapi, wgScriptPath);
			var wtperstasks = $('#main-persontasks');
			WTTracker.trackHover(wtperstasks, WTTracker.c.perstasks);
			wtptasks.display(wtperstasks);
			wtpexp.setPersonTask(wtptasks);
			wtptasks.setPersonExpertise(wtpexp);
		}
	}
	else if(wtcategories["Docu"]) {
		var wtdocu = new WTDocu(wgScriptPath);
		wtdocu.display($('#firstHeading'), $j("#main-docu"));
	}
	else if(wtcategories["Admin"]) {
		var wtadmin = new WTAdmin(wtapi, wgScriptPath, wtexpapi);
		wtadmin.display($('#firstHeading'), $j("#main-admin"));
	}

	var stdpropsdiv = $("#main-std-props");
	var factsdiv = $("#main-facts");
	var creditsdiv = $("#main-credits");
	if(!wtpagenotfound) {
		if(Object.keys(stdwtprops).length) {
			var wtstdprops = new WTStdProperties(wgPageName, allwtfacts, stdwtprops, wtutil, wtapi);
			wtstdprops.display(stdpropsdiv);
		}
		else {
			stdpropsdiv.css('display', 'none');
		}

		var wtfacts = new WTFacts(wgPageName, allwtfacts, stdwtprops, wtutil, wtapi);
		wtfacts.display(factsdiv);

		var wtcredits = new WTCredits(wgPageName, allwtdetails, wtutil, wtapi);
		wtcredits.display(creditsdiv);
	}
	else {
		stdpropsdiv.css('display', 'none');
		factsdiv.css('display', 'none');
		creditsdiv.css('display', 'none');
	}

	// Display category chooser
	//if(!Object.keys(wtcategories).length) {
	var catchooserdiv = $("#category-chooser");
	if(wtpagenotfound) {
		catchooserdiv.html(
			"<div style='padding:5px;color:red;font-weight:bold'>Uh oh, this page doesn't exist yet.</div>");
	}
	if(wtuid) {
		var wtcatchooser = new WTCategoryChooser(wgPageName, wtutil, wtapi);
		wtcatchooser.display(catchooserdiv);
	}
	else if(!wtpagenotfound && wtuid) {
		wtcatchooser.css('display', 'none');
	}
});
