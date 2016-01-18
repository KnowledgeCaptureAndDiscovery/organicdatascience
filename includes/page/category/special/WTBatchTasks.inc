<?php

class WTBatchTasks extends SpecialPage {
	
	public function __construct() {
		parent::__construct( 'WTBatchTasks' );
	}

	function execute( $par ) {
		$request = $this->getRequest();
		$out = $this->getOutput();
		$this->setHeaders();
 
		$tasksxml = $request->getText( 'tasksxml' );
		$file = "/tmp/tasks.xml";
		file_put_contents($file, $tasksxml);
		$users = $request->getText( 'users' );
		if($tasksxml) {
			if(!$users) {
				$out->addWikiText( 'No users provided to add tasks for' );
				return;
			}
			// Parse tasks 
			foreach(preg_split("/\s*,\s*/", $users) as $user) {
				list($uid, $uname) = preg_split("/\s*\|\s*/", $user);
				if($uname && $uid) {
					$out->addHtml("<br/>Adding.. <pre>$uid, $uname</pre>\n");
					new WTTraining($uname, $uid, $file);
				}
			}
		}
 
		// Show the form
		$html =  "<form method='POST'>\n";
		$html .= "Enter Userids: <input style='width:100%' name='users' value='$users' /><br/>\n";
		$html .= "<br/>Enter Tasks:<br/><textarea name='tasksxml' cols='50' rows='15'>$tasksxml</textarea>\n";
		$html .= "<br/><input type='submit'></input>\n";
		$html .= "</form>\n";
		$out->addHtml($html);
	}

	function getGroupName() {
		return "ODSGroup";
	}
}
