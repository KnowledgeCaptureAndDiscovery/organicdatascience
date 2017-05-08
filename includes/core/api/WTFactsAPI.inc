<?php
/**
 * API for adding and removing SMW Property Values to a Title
 *
 * @ingroup WorkflowTasks
 * @addtogroup API
 * @author varun
 */

global $ontNS;
$ontNS = "http://www.opmw.org/ontology/";

class WTFactsAPI extends ApiBase {
	private $facts = array();
	private $newdetails = null;

	private static $eventHandlers = array();

	const E_ADD = 'add';
	const E_DELETE = 'del';
	const E_UPDATE = 'replace';

	public function __construct($query, $moduleName) {
		parent :: __construct($query, $moduleName);
	}

	public function execute() {
		$params = $this->extractRequestParams();
		$error = '';
		$text = '';

		$op = $params['operation'];
		if (is_null($op))
			$this->dieUsage('Must specify operation', 'unspecified_operation');

		$title = $params['title'];
		if (is_null($title)) $this->dieUsage('Must specify page title', 'unspecified_title');

		$property = ucfirst($params['property']);
		$value = $params['value'];
		$totitle = $params['totitle'];
		$oldvalue = $params['oldvalue'];
		$subobjs = $params['subobjectsjson'] ? json_decode($params['subobjectsjson'], true) : null;
		$type = $params['type'];

		if ($op === 'newpage') {
			$categories = $params['categories'];
			if (is_null($categories)) $this->dieUsage('Must specify categories', 'unspecified_category');
			$catarray = $categories ? explode("|", $categories) : array();
			list($error, $text) = WTFactsAPI::createPageWithCategories($title, $catarray);
		} else if ($op === 'updatesubobjects') {
			list($error, $text, $data) = WTFactsAPI::updateSubobjects($title, $subobjs);
			list($error2, $this->facts, $text2) = WTFactsAPI::getFacts($title, $data);
		} elseif ($op === 'show') {
			list($error, $this->facts, $text) = WTFactsAPI::getFacts($title);
		} elseif ($op === 'add') {
			if (is_null($property)) $this->dieUsage('Must specify property', 'unspecified_property');
			if (is_null($value)) $this->dieUsage('Must specify value', 'unspecified_value');
			list($error, $text, $data, $this->newdetails) = WTFactsAPI::addFact($title, $property, $value, $type, true, $subobjs);
			list($error2, $this->facts, $text2) = WTFactsAPI::getFacts($title, $data);
		} elseif ($op === 'del') {
			if (is_null($property)) $this->dieUsage('Must specify property', 'unspecified_property');
			if (is_null($value)) $this->dieUsage('Must specify value', 'unspecified_value');
			list($error, $text, $data) = WTFactsAPI::delFact($title, $property, $value, $type, $subobjs);
			list($error2, $this->facts, $text2) = WTFactsAPI::getFacts($title, $data);
		} elseif ($op === 'set'){
			if (is_null($property)) $this->dieUsage('Must specify property', 'unspecified_property');
			if (is_null($value)) $this->dieUsage('Must specify value', 'unspecified_value');
			list($error, $text, $data) = WTFactsAPI::setPropertyValue($title, $property, $value);
			list($error2, $this->facts, $text2) = WTFactsAPI::getFacts($title, $data);
		} elseif ($op === 'replace'){
			if (is_null($property)) $this->dieUsage('Must specify property', 'unspecified_property');
			if (is_null($value)) $this->dieUsage('Must specify value', 'unspecified_value');
			list($error, $text, $data) = WTFactsAPI::replaceFact($title, $property, $value, $oldvalue);
			list($error2, $this->facts, $text2) = WTFactsAPI::getFacts($title, $data);
		} elseif ($op === 'move') {
			if (is_null($totitle)) $this->dieUsage('Must specify "to" page title', 'unspecified_property');
			list($error, $text, $data) = WTFactsAPI::moveWikiPage($title, $totitle);
			list($error, $this->facts, $text) = WTFactsAPI::getFacts($title, $data);
		}

		// Get suggested names
		global $wgTitle;
		$wgTitle = $title;
		$names = array();
		$inprops = WTFactsAPI::getInSubjectProperties($title);
		$cats = WTFactsAPI::getPageCategories($title);
		foreach($cats as $cat) {
			$wtnaming = new WTPageNaming($cat, $this->facts, $inprops);
			$pageid = $wtnaming->getPageId();
			if($pageid)
				$names[] = $pageid;
		}

		if ($error === '')
			$error = 'UnknownOperation';

		// Execute any event handlers
		if($op && $title && $property)
			WTFactsAPI::triggerEvents($op, $title, $property, $value);

		$result=array();
		$result['result']=$error;
		$result['operation']=$op;
		if ($title) $result['page'] = $title;
		if ($text) $result['text'] = $text;
		foreach($this->facts as $prop => $values) {
			$this->getResult()->setIndexedTagName( $this->facts[$prop], 'value');
		}
		$result['facts'] = $this->facts;
		$result['names'] = $names;

		if($this->newdetails) {
			$result['newdetails'] = $this->newdetails;
		}

		$this->getResult()->addValue(null, 'wtfacts', $result);
	}

	public static function addEventHandler($propid, $type, $callback) {
		$prop = SMWDIProperty::newFromUserLabel( $propid );
		$propid = $prop->getKey();
		if(array_key_exists($propid, WTFactsAPI::$eventHandlers)) {
			if(array_key_exists($type, WTFactsAPI::$eventHandlers[$propid]))
				WTFactsAPI::$eventHandlers[$propid][$type][] = $callback;
			else
				WTFactsAPI::$eventHandlers[$propid][$type] = [$callback];
		}
		else
			WTFactsAPI::$eventHandlers[$propid] = array( $type => [$callback] );
	}

	public static function deleteEventHandler($propid, $type, $callback) {
		$prop = SMWDIProperty::newFromUserLabel( $propid );
		$propid = $prop->getKey();
		if(array_key_exists($propid, WTFactsAPI::$eventHandlers))
			if(array_key_exists($type, WTFactsAPI::$eventHandlers[$propid]))
				if(($key = array_search($callback, WTFactsAPI::$eventHandlers[$propid][$type])) !== false) {
				    unset(WTFactsAPI::$eventHandlers[$propid][$type][$key]);
				}
	}

	public static function triggerEvents($op, $title, $propid, $value) {
		$prop = SMWDIProperty::newFromUserLabel( $propid );
		$propid = $prop->getKey();
		if(array_key_exists($propid, WTFactsAPI::$eventHandlers)) {
			if(array_key_exists($op, WTFactsAPI::$eventHandlers[$propid])) {
				foreach(WTFactsAPI::$eventHandlers[$propid][$op] as $callback) {
					call_user_func($callback, $title, $propid, $value);
				}
			}
		}
	}

	public static function getDVText($dataValue) {
		$text = $dataValue->getShortText(null);
		return $text;
	}

	public static function getDIPage($di) {
		$subobj = $di->getSubobjectName();
		$pagekey = ($di->getTitle() ? $di->getTitle()->getPrefixedDBKey() : $di->getDBKey());
		if($subobj)
			return "$pagekey#$subobj";
		return $pagekey;
	}

	private static function getDateString($di) {
		$year = str_pad($di->getYear(), 4, "0", STR_PAD_LEFT);
		$month = str_pad($di->getMonth(), 2, "0", STR_PAD_LEFT);
		$day = str_pad($di->getDay(), 2, "0", STR_PAD_LEFT);
		return "$year-$month-$day";
	}

	public static function getDIText($di) {
		switch ( $di->getDIType() ) {
			case SMWDataItem::TYPE_TIME:
				return WTFactsAPI::getDateString($di);
				//return DateTime::createFromFormat('Y-m-d', $di->getMwTimestamp(), new DateTimeZone('UTC'));
			case SMWDataItem::TYPE_NUMBER:
				return $di->getNumber();
			case SMWDataItem::TYPE_URI:
				return $di->getURI();
			case SMWDataItem::TYPE_WIKIPAGE:
				return WTFactsAPI::getDIPage($di);
			default:
				return $di->getSerialization();
		}
	}

	public static function getDVDetails($dv, $di) {
		$text = WTFactsAPI::getDVText($dv);
		$val = WTFactsAPI::getDIText($di);
		$type = get_class($di);
		$type = str_replace("SMW\DI","", $type);
		$type = str_replace("SMWDI","", $type);
		$key = ($type=="WikiPage") ? WTFactsAPI::getDIPage($di) : "";
		$exists = ($type=="WikiPage") ? $di->getTitle()->exists() : "";
		return array("val"=>$val, "text"=>$text, "type"=>$type, "key"=>$key, "exists"=>$exists, "_BC_bools"=>array("exists"));
	}

	public static function setFactAuthors($title, $facts, $data) {
		$dbr = wfGetDB( DB_SLAVE );
		$titObj = Title::newFromText($title);
		$title = $titObj->getDbKey();
		$props = $data->getProperties();
		$title = mysql_escape_string($title);
		$res = $dbr->query(
			"SELECT rev_user_text, rev_user, rev_comment FROM revision a LEFT JOIN page b ON a.rev_page=b.page_id WHERE page_title='$title' AND (rev_comment LIKE '%Added PropertyValue%' OR rev_comment LIKE '%Set PropertyValue%' OR rev_comment LIKE '%WTLiPD:%' OR rev_comment LIKE '%Bootstrap:%') ORDER BY rev_timestamp DESC"
		);
		$auths = array();
		$initial_author = 'Anonymous';
		if( $res && $dbr->numRows( $res ) > 0 ) {
			while( $row = $dbr->fetchObject( $res ) ) {
				if(preg_match_all("/(Added|Set) PropertyValue: (.+?) = (.+)/", $row->rev_comment, $allm, PREG_SET_ORDER)) {
					foreach($allm as $m) {
						$prop = ucfirst(preg_replace('/\s/','_',$m[2]));
						$propDI = array_key_exists($prop, $props) ? $props[$prop] : null;
						if(!$propDI) continue;
						$sprop = strtolower($prop);
						$val = $m[3];
						$val = preg_replace("/^http:\/?\/?/i", "", $val);
						$val = preg_replace("/^mailto:/i", "", $val);
						$valDV = SMWDataValueFactory::newTypeIdValue ($propDI->findPropertyTypeId(), $val);
						$val = WTFactsAPI::getDIText($valDV->getDataItem());
						$val = strtolower($val);
						$val = preg_replace("/\/$/", "", $val);
						$val = preg_replace("/ /", "_", $val);
						$val = md5($val);
						if(array_key_exists("$sprop||$val", $auths) && $auths["$sprop||$val"])
							continue;
						$user = $row->rev_user > 0 ? $row->rev_user_text : 'Anonymous';
						$auths["$sprop||$val"] = $user;
					}
				}
				else if(preg_match("/^(WTLiPD|Bootstrap):/", $row->rev_comment, $allm)) {
					$initial_author = $row->rev_user > 0 ? $row->rev_user_text : 'Anonymous';
				}
			}
		}

		$newfacts = array();
		foreach($facts as $prop=>$pd) {
			$sprop = preg_replace("/ /", "_", strtolower($prop));
			$newvals = array();
			foreach($pd["values"] as $valobj) {
				$val = strtolower($valobj["val"]);
				//$val = preg_replace("/^http:\/?\/?/i", "", $val);
				//$val = preg_replace("/^mailto:/i", "", $val);
				$val = preg_replace("/\/$/", "", $val);
				$val = preg_replace("/ /", "_", $val);
				$val = md5($val);
				$author = isset($auths["$sprop||$val"]) ? $auths["$sprop||$val"] : $initial_author;
				$valobj["author"] = $author;
				$newvals[] = $valobj;
			}
			$newfacts[$prop] = array("exists"=>$pd["exists"], "values"=>$newvals);
			$newfacts[$prop]["_BC_bools"] = array("exists");
		}

		return $newfacts;
	}

	public static function getFacts($title, $data=null) {
		global $wgUseSimpleTasks, $wgCore;
		$blacklist = array(
				"Answer", "SubTask", "Answered", "Workflow",
				"WorkflowDataVariable", "WorkflowParamVariable", "WorkflowProcessVariable",
				"WorkflowContributor", "WorkflowDataArtifact", "WorkflowInputDataArtifact",
				"WorkflowParamArtifact", "WorkflowProcess", "ExecutedWorkflow", "WorkflowCreatedIn",
				"WorkflowEndTime", "WorkflowExecutedIn", "WorkflowStartTime", "WorkflowStatus",
				"WorkflowTemplate", "WorkflowWingsTemplate", "WorkflowWingsExecution",
				"WorkflowUser", "WorkflowRunImage", "WorkflowTemplateImage", "DataHasLocation",
				"DataHasPubbyLink", "DataHasType", "DataUsedForVariable");

		if(!$wgUseSimpleTasks)
				$blacklist = array_merge($blacklist,
					array("Type", "Progress", "Owner", "Participants", "Expertise", "StartDate", "TargetDate"));

		if(!$data) {
			$titObj = Title::newFromText($title);
			$subject = SMWDIWikiPage::newFromTitle( $titObj );
			$data = WTFactsAPI::getSemanticData( $subject );
		}

		$subobjects = array();
		$subdatas = $data->getSubSemanticData();
		if($subdatas) {
			foreach($subdatas as $subdata) {
				$subname = $subdata->getSubject()->getSubobjectName();
				$subid = "$title#$subname";
				$sofacts = WTFactsAPI::getFacts($subid, $subdata);
				if($sofacts)
					$subobjects[$subname] = $sofacts[1];
			}
		}

		$facts = array();
		$props = $data->getProperties();
		foreach($props as $prop) {
			$pkey = $prop->getKey();
			if($pkey[0] == "_") continue;
			if(in_array($prop->getLabel(), $blacklist)) continue;
			$ptitle = Title::newFromText("Property:".$pkey);
			$propexists = $ptitle->exists();
			if($prop->getLabel()) {
				$valobjs = $data->getPropertyValues($prop);
				$vals = array();
				foreach ( $valobjs as $di ) {
					$dv = SMWDataValueFactory::newDataItemValue( $di, $prop );
					$vals[] = WTFactsAPI::getDVDetails($dv, $di);
				}
				$facts[$prop->getLabel()] = array("exists"=>$propexists, "values"=>$vals);
				$facts[$prop->getLabel()]["_BC_bools"] = array("exists");
			}
		}
		$facts = WTFactsAPI::setFactAuthors($title, $facts, $data);
		$facts['subobjects'] = $subobjects;
		return array( "Success", $facts, "Facts for $title");
	}

	public static function getPageCategories($pageid) {
		$categories = array();
		$titObj = Title::newFromText($pageid);
		$subject = SMWDIWikiPage::newFromTitle( $titObj );
		$article = new Article($titObj);
		foreach($article->getCategories() as $cat) {
			$categories[] = $cat->getDbKey();
		}
		return $categories;
	}

	public static function getCategoryProperties($cat) {
		$titObj = Title::newFromText("Category:".$cat);
		$subject = SMWDIWikiPage::newFromTitle( $titObj );
		$data = WTFactsAPI::getSemanticData( $subject );

		$props = array();

		// Get parent category properties
		/*$article = new Article($titObj);
		foreach($article->getCategories() as $parentcat) {
			$pprops = WTFactsAPI::getCategoryProperties($parentcat->getDbKey());
			$props = array_merge($props, $pprops);
		}*/

		//$subprop = SMWDIProperty::newFromUserLabel( "Subproperty_of" );
		//$pprops = $data->getPropertyValues($prop);
		// Get this category properties
		$prop = SMWDIProperty::newFromUserLabel( "Has_property" );
		$valobjs = $data->getPropertyValues($prop);
		foreach ( $valobjs as $propDI ) {
			$prop = WTFactsAPI::getPropertyDetails($propDI->getDbKey());
			$props[$prop['label']] = $prop;
		}
		return $props;
	}

	private static function getParentProperty($propDI) {
	}

	public static function getPropertyDetails($plabel) {
		$pdetails = array();
		$propDI = SMWDIProperty::newFromUserLabel( $plabel );

		$titObj = Title::newFromText("Property:".$plabel);
		$subject = SMWDIWikiPage::newFromTitle( $titObj );
		$data = WTFactsAPI::getSemanticData( $subject );

		$proptype  = $propDI->findPropertyTypeId();
		$pdetails['label'] = $propDI->getLabel();
		$pdetails['exists'] = $titObj->exists();
		$pdetails['type'] = $proptype;

		if($proptype == "_wpg") {
			$catprop = SMWDIProperty::newFromUserLabel( "Has_page_category" );
			$valobjs = $data->getPropertyValues($catprop);
			foreach ( $valobjs as $di ) {
				$pdetails['category'] = $di->getDbKey();
			}
		}
		$iconprop = SMWDIProperty::newFromUserLabel( "Fontawesome_icon" );
		$valobjs = $data->getPropertyValues($iconprop);
		foreach ( $valobjs as $di ) {
			$pdetails['icon'] = WTFactsAPI::getDIText($di);
		}
		$multivals = SMWDIProperty::newFromUserLabel( "Has_multiple_values" );
		$valobjs = $data->getPropertyValues($multivals);
		foreach ( $valobjs as $di ) {
			$pdetails['multivalues'] = WTFactsAPI::getDIText($di);
		}
		$parentprop = SMWDIProperty::newFromUserLabel( "Parent_property" );
		$parentobjs = $data->getPropertyValues($parentprop);
		foreach ( $parentobjs as $di ) {
			$pdetails['parent'] = $di->getDbKey();
		}
		$pdetails["_BC_bools"] = array("exists", "multivalues");
		return $pdetails;
	}

	public static function processSubobjects($subobjs, $data) {
		if(!$subobjs) return;
		$title = $data->getSubject()->getDBKey();
		foreach($subobjs as $subobj) {
			$subid = $title."#".$subobj['id'];
			$subdata = $data->findSubSemanticData($subobj['id']);
			if($subdata) {
				$data->removeSubSemanticData($subdata);
			}
			$subsubject = SMWDIWikiPage::newFromTitle(Title::newFromText($subid));
			$subdata = new SMWSemanticData($subsubject);
			foreach($subobj['properties'] as $subprop=>$subval) {
				$subpropDI = SMWDIProperty::newFromUserLabel( $subprop );
				$subvalDV = SMWDataValueFactory::newPropertyObjectValue( $subpropDI, $subval );
				$subvalDI = $subvalDV->getDataItem();
				WTFactsAPI::addPropValToData($subdata, $subpropDI, $subvalDI);
			}
			$data->addSubSemanticData($subdata);
		}
	}

	public static function updateSubobjects($title, $subobjs) {
		$subTitle = Title::newFromText($title);
		$subject = SMWDIWikiPage::newFromTitle( $subTitle );
		$data = WTFactsAPI::getSemanticData( $subject );
		// Process subobject data if present
		WTFactsAPI::processSubobjects($subobjs, $data);
		$summary = "Updated Subobjects";
		$ret = WTFactsAPI::updateWiki($title, $summary, $data);
		return $ret;
	}

	public static function replaceSpecialCharacters($text){
		$text = preg_replace("/\//", " or ", $text);
		$text = preg_replace('/[^A-Za-z0-9\-\_ ]/', ' ', $text); // replace all special chars with a space
		return preg_replace ("/ +/", " ", $text); // remove multiple spaces
	}

	public static function addFact($title, $property, $value, $type=null, $create_prop=true, $subobjs=null) {
		global $wgCore;
		if($property == "SubTask")
			$value = WTFactsAPI::replaceSpecialCharacters($value);
		$ptitle = Title::newFromText("Property:".$property);
		if(!$ptitle->exists() && $create_prop) {
			$type = WTFactsAPI::guessValueType($value);
			$text = "Property type: [[has type::$type]]\n";
			$summary = "Creating new Property";
			WTFactsAPI::createWikiPage("Property:$property", $text, $summary);
		}

		$subTitle = Title::newFromText($title);
		$subject = SMWDIWikiPage::newFromTitle( $subTitle );
		$data = WTFactsAPI::getSemanticData( $subject );

		$propDI = SMWDIProperty::newFromUserLabel( $property );
		$valDV = SMWDataValueFactory::newPropertyObjectValue( $propDI, $value );
		$valDI = $valDV->getDataItem();

		if($valDI->getDIType() == SMWDataItem::TYPE_ERROR)
			return array( "Error", $valDI->getErrors(), $data, null );

		$data = WTFactsAPI::addPropValToData($data, $propDI, $valDI);

		// Process subobject data if present
		WTFactsAPI::processSubobjects($subobjs, $data);

		if($property == "Workflow") {
			$data = WTFactsAPI::addFactsFromWorkflowPage($title, $subject, $data, $value);
			$summary = "Added Workflow Properties from $value";
			$ret = WTFactsAPI::updateWiki($title, $summary, $data);

			$wflow = new Workflow(Title::newFromText($title));
			$ret[] = $wflow->getPageDetails();
		}
		else if($property == "ExecutedWorkflow") {
			$data = WTFactsAPI::addFactsFromExecutedWorkflowPage($title, $subject, $data, $value);
			$summary = "Added Executed Workflow Properties from $value";
			$ret = WTFactsAPI::updateWiki($title, $summary, $data);

			$wflow = new ExecutedWorkflow(Title::newFromText($title));
			$ret[] = $wflow->getPageDetails();
		}
		else {
			$summary = "Added PropertyValue: $property = $value";
			$ret = WTFactsAPI::updateWiki($title, $summary, $data);

			if($property == "SubTask") {
				$subcat = "";
				foreach($data->getPropertyValues(SMWDIProperty::newFromUserLabel("_INST")) as $catobj)
					$subcat = $catobj->getDbKey();
				$valtitle = Title::newFromText($value);
				if(!$valtitle->exists() && $subcat)
					WTFactsAPI::createPageWithCategory($value, $subcat);
				$task = new Task($valtitle);
				$ret[] = $task->getPageDetails();
			}
			else if($property == "Answer") {
				$antitle = Title::newFromText($value);
				if(!$antitle->exists())
					WTFactsAPI::createPageWithCategory($value, "Answer");
				$item = array("text"=>$antitle->getText(),
							"exists"=>$antitle->exists(),
							"_BC_bools"=>array("exists"),
							"key"=>$antitle->getDbKey());
				$ret[] = array("item"=>$item);
			}
			else if($property == "DataLocation") {
				$page = new WTUserDescribedData(Title::newFromText($title));
				$ret[] = $page->getPageDetails();
			}
			else if($property == "DataWikiLocation") {
				$page = new WTUserProvidedData(Title::newFromText($title));
				$ret[] = $page->getPageDetails();
			}
			else if($property == "HasFileName $wgCore") {
				$page = new WTUserProvidedData(Title::newFromText($title), "HasFileName $wgCore");
				$ret[] = $page->getPageDetails();
			}
			else if($property == "Columns") {
				$valtitle = Title::newFromText($value);
				if(!$valtitle->exists())
					WTFactsAPI::createPageWithCategory($value, "DataType");
				$ret[] = null;
			}
			else {
				$ret[] = null;
			}
		}
		return $ret;
	}

	public static function delFact($title, $property, $value, $type=null, $subobjs=null) {
		$subTitle = Title::newFromText($title);
		$subject = SMWDIWikiPage::newFromTitle( $subTitle );
		$data = WTFactsAPI::getSemanticData( $subject );

		$propDI = SMWDIProperty::newFromUserLabel( $property );
		$valDV = SMWDataValueFactory::newPropertyObjectValue( $propDI, $value );
		$valDI = $valDV->getDataItem();

		if($property == "Workflow") {
			$props = array("Workflow", "WorkflowDataVariable", "WorkflowParamVariable",
				"WorkflowContributor", "WorkflowProcessVariable", "WorkflowTemplateImage",
				"WorkflowWingsTemplate", "WorkflowCreatedIn");
			foreach($props as $prop) {
				$propDI = SMWDIProperty::newFromUserLabel( $prop );
				$data = WTFactsAPI::removePropValsFromData($data, $propDI);
			}
			$summary = "Removed Workflow Properties from $value";
			return WTFactsAPI::updateWiki($title, $summary, $data);
		}
		if($property == "ExecutedWorkflow") {
			$props = array("ExecutedWorkflow", "WorkflowEndTime", "WorkflowExecutedIn",
				"WorkflowStartTime", "WorkflowStatus", "WorkflowTemplate",
				"WorkflowWingsExecution", "WorkflowUser", "WorkflowDataArtifact", "WorkflowInputDataArtifact",
				"WorkflowParamArtifact", "WorkflowProcess", "WorkflowTemplateImage", "WorkflowRunImage");

			$datasets = array();
			$dataProp = SMWDIProperty::newFromUserLabel("WorkflowDataArtifact");
			$idataProp = SMWDIProperty::newFromUserLabel("WorkflowInputDataArtifact");
			$dataitems = $data->getPropertyValues($dataProp);
			$idataitems = $data->getPropertyValues($idataProp);
			$dataitems = array_merge($dataitems, $idataitems);
			foreach($dataitems as $di) {
				$dtitle = Title::newFromText($di->getDbKey());
				if($dtitle->exists()) $datasets[] = $di->getDbKey();
			}
			foreach($props as $prop) {
				$propDI = SMWDIProperty::newFromUserLabel( $prop );
				$data = WTFactsAPI::removePropValsFromData($data, $propDI);
			}
			$summary = "Removed Executed Workflow Properties from $value";
			$ret = WTFactsAPI::updateWiki($title, $summary, $data);
			WTFactsAPI::removeOrphanDatasetPages($datasets, $dataProp);
			WTFactsAPI::removeOrphanDatasetPages($datasets, $idataProp);
			return $ret;
		}
		else {
			$data = WTFactsAPI::removePropValFromData($data, $propDI, $valDI);
			// Process subobject data if present
			WTFactsAPI::processSubobjects($subobjs, $data);

			$summary = "Deleted PropertyValue: $property = $value";
			return WTFactsAPI::updateWiki($title, $summary, $data);
		}
	}


	public static function setPropertyValue($title, $property, $value) {
		return WTFactsAPI::setFactsHelper($title, $property, $value, true);
	}

	public static function replaceFact($title, $property, $value, $oldvalue) {
		return WTFactsAPI::setFactsHelper($title, $property, $value, false, $oldvalue);
	}

	public static function modifySemanticData($data, $property, $value, $replaceAll, $oldvalue=null) {
		$propDI = SMWDIProperty::newFromUserLabel( $property );
		$valDI = null;
		$oldValDI = null;
		if($value != "") {
			$valDV = SMWDataValueFactory::newPropertyObjectValue( $propDI, $value );
			$valDI = $valDV->getDataItem();
		}
		if($oldvalue != null) {
			$oldValDV = SMWDataValueFactory::newPropertyObjectValue( $propDI, $oldvalue );
			$oldValDI = $oldValDV->getDataItem();
		}
		$data = WTFactsAPI::setPropValInData($data, $propDI, $valDI, $replaceAll, $oldValDI);
		return $data;
	}

	public static function setFactsHelper($title, $property, $value, $replaceAll, $oldvalue=null) {
		if($property == "SubTask")
			$value = WTFactsAPI::replaceSpecialCharacters($value);
		$subTitle = Title::newFromText($title);
		$subject = SMWDIWikiPage::newFromTitle( $subTitle );
		$data = WTFactsAPI::getSemanticData( $subject );
		$data = WTFactsAPI::modifySemanticData($data, $property, $value, $replaceAll, $oldvalue);
		$summary = "Set PropertyValue: $property = $value";
		return WTFactsAPI::updateWiki($title, $summary, $data);
	}


	public static function addFactsMultiple($title, $pvarray, $create_prop=true) {
		$subTitle = Title::newFromText($title);
		$subject = SMWDIWikiPage::newFromTitle( $subTitle );
		$data = WTFactsAPI::getSemanticData( $subject );

		//$summaries = array();
		foreach($pvarray as $pv) {
			$property = $pv["prop"];
			$value = $pv["value"];
			$ptitle = Title::newFromText("Property:".$property);
			$type = null;
			if(!$ptitle->exists() && $create_prop) {
				$type = WTFactsAPI::guessValueType($value);
				$text = "Property type: [[has type::$type]]\n";
				$summary = "Creating new Property";
				WTFactsAPI::createWikiPage("Property:$property", $text, $summary);
			}

			if(preg_match('/^http[s]?:\/\//', $value))
				$value = preg_replace('/^http[s]?:\/\//', '', $value);

			$propDI = SMWDIProperty::newFromUserLabel( $property );
			$valDV = SMWDataValueFactory::newPropertyObjectValue( $propDI, $value );
			$valDI = $valDV->getDataItem();
			$data = WTFactsAPI::addPropValToData($data, $propDI, $valDI);

			$summary = "Added PropertyValue: $property = $value";
			$ret = WTFactsAPI::updateWiki($title, $summary, $data);
			//$summaries[] = "Added PropertyValue: $property = $value";
		}
		//$summary = "Added Multiple PropertyValues";
		//$ret = WTFactsAPI::updateWiki($title, $summaries, $data);
		return $ret;
	}

	public static function addPropValToData($data, $propDI, $valDI, $createValuePage=true) {
		$valDI = WTFactsAPI::fixDI($valDI);
		if($createValuePage)
			WTFactsAPI::createValuePage($propDI, $valDI);
		$data->addPropertyObjectValue( $propDI, $valDI );
		return $data;
	}

	public static function updateWiki($title, $summary, $data) {
		$article = new Article(Title::newFromText($title));
		$text = $article->getContent();
		$doc = new POMPage($text);

		$facts = null;
		$setobj = array_key_exists('#set', $doc->templates) ? $doc->templates['#set'] : array();
		if($setobj && is_array($setobj)) {
			$facts = $setobj[0];
			while($facts->getParametersCount())
				$facts->removeParameterByNumber(0);
		}
		else {
			$facts = new POMTemplate("{{#set:}}");
			/*$doc->addChild(new POMTextNode("\n"));
			$doc->addChild(new POMTextNode("<!-- Add any wiki Text above this Line -->"));
			$doc->addChild(new POMTextNode("\n"));
			$doc->addChild(new POMTextNode("<!-- Do NOT Edit below this Line -->"));
			$doc->addChild(new POMTextNode("\n"));*/
			$doc->addChild($facts);
			/*$doc->addChild(new POMTextNode("\n"));*/
		}

		// Write property-values to POMPage
		foreach ( $data->getProperties() as $property ) {
			$propkey = $property->getKey();
			if($propkey[0] == "_") continue;
			$values = $data->getPropertyValues( $property );
			//print "$propkey = ".implode(array_map("WTFactsAPI::getDIText", $values), ",")."\n";
			foreach ( array_unique($values, SORT_REGULAR) as $dataItem ) {
				$facts->addParameter( "\n\t".$property->getLabel(), WTFactsAPI::getDIText($dataItem) );
			}
		}

		// Write subobjects to POMPage
		$subobjpoms = array_key_exists('#subobject', $doc->templates) ? $doc->templates['#subobject'] : array();
		$nchildren = array();
		for($i=0; $i<sizeof($doc->children);$i++) {
			if(!in_array($doc->children[$i], $subobjpoms))
				$nchildren[] = $doc->children[$i];
		}
		$doc->children = $nchildren;

		$subdatas = $data->getSubSemanticData();
		foreach($subdatas as $subdata) {
			$subname = $subdata->getSubject()->getSubobjectName();
			$subobjpom = new POMTemplate("{{#subobject:$subname}}");
			foreach ( $subdata->getProperties() as $subprop ) {
				$subpkey = $subprop->getKey();
				if($subpkey[0] == "_") continue;
				$subvalues = $subdata->getPropertyValues( $subprop );
				//print "$subname: $subpkey = ".implode(array_map("WTFactsAPI::getDIText", $subvalues), ",")."\n";
				foreach ( array_unique($subvalues, SORT_REGULAR) as $subdataItem ) {
					$subobjpom->setParameter( $subprop->getLabel(), WTFactsAPI::getDIText($subdataItem) );
				}
			}
			$doc->addChild($subobjpom);
		}

		// Create new wiki text from POMPage
		$newtext = $doc->asString();
		//print "$newtext\n";
		if(strpos($newtext, "{{#set}}") !== false) {
			$newtext = str_replace("{{#set}}", "{{#set:}}", $newtext);
		}

		// If summary is an array, do multiple edits with all summary items
		if(is_array($summary)) {
			foreach($summary as $sum) {
				$status = $article->doEdit($newtext, $sum);
				if (!$status->isOK())
					return array( "Error", "Something went wrong: " . $status->getWikiText(), $data );
			}
			return array( "Success", $summary, $data );
		}
		else {
			$status = $article->doEdit($newtext, $summary);
			if ($status->isOK()) {
				return array( "Success", $summary, $data );
			} else {
				return array( "Error", "Something went wrong: " . $status->getWikiText(), $data );
			}
		}
	}

	public static function createPageWithCategory($title, $category) {
		$categories = $category ? array($category) : array();
		return WTFactsAPI::createPageWithCategories($title, $categories);
	}

	public static function createPageWithCategories($title, $categories) {
		$subTitle = Title::newFromText($title);
		$text = "";
		$summary = "";
		if($subTitle->exists()) {
			$subject = SMWDIWikiPage::newFromTitle( $subTitle );
			$data = WTFactsAPI::getSemanticData( $subject );
			/*$cats = array();
			foreach($data->getPropertyValues(SMWDIProperty::newFromUserLabel("_INST")) as $catobj)
				$cats[] = $catobj->getDbKey();*/
			$text = (new Article($subTitle))->getContent();
			$text = preg_replace("/\[\[\s*Category:.+?\]\](\\n)?/", "", $text);
		}
		$summary = "Setting categories as ". implode(", ", $categories);
		foreach(array_reverse($categories) as $category)
			$text = "[[Category:$category]]\n$text";
		return WTFactsAPI::createWikiPage($title, $text, $summary);
	}

	public static function createWikiPage($title, $text, $summary) {
		$titobj = Title::newFromText($title);
		$content = ContentHandler::makeContent($text, $titobj);
		$page = new WikiPage($titobj);
		$status = $page->doEditContent($content, $summary);
		if ($status->isOK()) {
			return array( "Success", $summary );
		} else {
			return array( "Error", "Something went wrong: " . $status->getWikiText() );
		}
	}

	public static function deleteWikiPage($title, $reason) {
		$article = new Article(Title::newFromText($title));
		$article->doDelete($reason);
	}

	public static function moveWikiPage($from, $to) {
		$fromtitle = Title::newFromText($from);
		$totitle = Title::newFromText($to);
		// Get Incoming properties
		$inprops = WTFactsAPI::getInSubjectProperties($from);
		// TODO: Move without redirect
		$fromtitle->moveTo($totitle, true, 'Moved via suggestion', false);
		// Replace incomping property values to the new page id
		foreach($inprops as $inprop) {
			WTFactsAPI::replaceFact($inprop["subject"], $inprop["property"], $to, $from);
		}
	}

	public static function getInSubjectProperties($object) {
		$store = &smwfGetStore();
    $title = Title::newFromText($object);
    $valpage = SMWDIWikiPage::newFromTitle($title);
    $inproperties = $store->getInProperties($valpage);
    $propsubjects = array();
    foreach ( $inproperties as $propDI ) {
        $values = $store->getPropertySubjects( $propDI, $valpage );
        foreach ( $values as $value )
            $propsubjects[] = array("subject"=>$value->getDbKey(), "property"=>$propDI->getLabel());
    }
    return $propsubjects;
	}

	/* --- private methods --- */

	private static function getSemanticData($subject) {
		$store = &smwfGetStore();
		$data = $store->getSemanticData($subject);
		WTFactsAPI::fetchSubobjectData($data);
		return $data;
	}

	private static function fetchSubobjectData($data) {
		$store = &smwfGetStore();
		$title = $data->getSubject()->getDBKey();
		foreach ( $data->getProperties() as $property ) {
			$pkey = $property->getKey();
			if($pkey == "_SOBJ") {
				$subobjs = $data->getPropertyValues($property);
				foreach($subobjs as $subobj) {
					$subid = $title."#".$subobj->getSubobjectName();
					$subdata = $data->findSubSemanticData($subid);
					if(!$subdata || empty($subdata)) {
						$subtitObj = Title::newFromText($subid);
						$subsubject = SMWDIWikiPage::newFromTitle( $subtitObj );
						$subdata = $store->getSemanticData($subsubject);
						$data->addSubSemanticData($subdata);
					}
				}
			}
		}
		return $data;
	}

	// Create a page for the value it should belong to a category
	private static function createValuePage($propDI, $valDI) {
		$details = WTFactsAPI::getPropertyDetails($propDI->getLabel());
		$cat = array_key_exists("category", $details) ? $details["category"] : null;
		if($details["type"] == "_wpg" && $cat) {
			$title = Title::newFromText($valDI->getDbKey());
			if(!$title->exists()) {
				$text = $cat ? "[[Category:$cat]]\n" : "";
				$summary = "Creating new page";
				WTFactsAPI::createWikiPage($valDI->getDbKey(), $text, $summary);
			}
		}
	}


	// FIXME: Hack to fix hierpart for URIs
	private static function fixDI($valDI) {
		if($valDI->getDIType() != SMWDataItem::TYPE_URI) return $valDI;
		if($valDI->getScheme() == "http" || $valDI->getScheme() == "https") {
			$hierpart = preg_replace('/^\/\//', '', $valDI->getHierpart());
			$valDI = new SMWDIUri($valDI->getScheme(),
							$hierpart,
							$valDI->getQuery(),
							$valDI->getFragment());
		}
		return $valDI;
	}

	private static function removePropValFromData($data, $propDI, $valDI) {
		$valDI = WTFactsAPI::fixDI($valDI);
		$data->removePropertyObjectValue($propDI, $valDI);
		return $data;
	}

	private static function setPropValInData($data, $propDI, $valDI, $replaceAll, $oldValDI) {
		if($replaceAll)
			$data = WTFactsAPI::removePropValsFromData($data, $propDI);
		else if($oldValDI)
			$data = WTFactsAPI::removePropValFromData($data, $propDI, $oldValDI);

		if($valDI)
			$data = WTFactsAPI::addPropValToData($data, $propDI, $valDI);

		return $data;
	}

	private static function removePropValsFromData($data, $propDI) {
		foreach($data->getPropertyValues($propDI) as $valDI) {
			$data->removePropertyObjectValue($propDI, $valDI);
		}
		return $data;
	}

	private static function getPValue($arr, $key) {
		if(array_key_exists($key, $arr))
			return $arr[$key];
		return null;
	}

	private static function addFactsFromWorkflowPage($title, $subject, $data, $url) {
		global $IP;
		global $ontNS;
		include_once( "$IP/lib/arc2/ARC2.php" );

		$url = preg_replace("/http:\/\/opmw/", "http://www.opmw", $url);
		$url = preg_replace("/export\/resource/", "export/page/resource", $url);
		$objuri = preg_replace("/page\/resource/", "resource", $url);
		$rdfuri = preg_replace("/page\/resource/", "data/resource", $url);
		$rdfuri = $rdfuri."?output=xml&all=1";
		$parser = ARC2::getRDFParser();
		$parser->parse( $rdfuri );
		$index = $parser->getSimpleIndex();
		$pvals = $index[$objuri];
		$processes = WTFactsAPI::getPValue($pvals, "http://openprovenance.org/model/opmo#hasProcess");
		$artifacts = WTFactsAPI::getPValue($pvals, "http://openprovenance.org/model/opmo#hasArtifact");
		$contributors = WTFactsAPI::getPValue($pvals, "http://purl.org/dc/terms/contributor");
		$timages = WTFactsAPI::getPValue($pvals, $ontNS."hasTemplateDiagram");
		$tfiles = WTFactsAPI::getPValue($pvals, $ontNS."hasNativeSystemTemplate");
		$tfile = $tfiles ? $tfiles[0] : null;
		$csys = WTFactsAPI::getPValue($pvals, $ontNS."createdInWorkflowSystem");

		foreach($contributors as $uri)
			$data = WTFactsAPI::addPV($subject, $data, "WorkflowContributor", "_uri", $uri, true);
		foreach ($timages as $timage)
			$data = WTFactsAPI::addPV($subject, $data, "WorkflowTemplateImage", "_uri", $timage, true);
		if($tfile)
			$data = WTFactsAPI::addPV($subject, $data, "WorkflowWingsTemplate", "_uri", $tfile, true);
		if($csys)
			foreach ($csys as $cs)
				$data = WTFactsAPI::addPV($subject, $data, "WorkflowCreatedIn", "_uri", $cs, true);

		//print_r($index);
		foreach($index as $key=>$val) {
			#$templates = $val["http://www.opmw.org/ontology/template"];
			#if($templates && in_array($objuri, $templates)) {
				$types = WTFactsAPI::getPValue($val, "http://www.w3.org/1999/02/22-rdf-syntax-ns#type");
				if(!$types) continue;
				if(in_array($ontNS."WorkflowTemplateArtifact", $types)) {
					if(in_array($ontNS."ParameterVariable", $types)) {
						$data = WTFactsAPI::addPV($subject, $data, "WorkflowParamVariable", "_uri", $key, true);
					}
					else { //if(in_array($ontNS."DataVariable", $types))
						$data = WTFactsAPI::addPV($subject, $data, "WorkflowDataVariable", "_uri", $key, true);
					}
					//print "Artifact: $key\n";
				}
				else if(in_array($ontNS."WorkflowTemplateProcess", $types)) {
					//print "Process: $key\n";
					$data = WTFactsAPI::addPV($subject, $data, "WorkflowProcessVariable", "_uri", $key, true);
				}
			#}
		}
		return $data;
	}

	private static function addFactsFromExecutedWorkflowPage($title, $subject, $data, $url) {
		global $IP;
		global $ontNS;
		include_once( "$IP/lib/arc2/ARC2.php" );

		set_time_limit (0);

		$url = preg_replace("/http:\/\/opmw/", "http://www.opmw", $url);
		$url = preg_replace("/export\/resource/", "export/page/resource", $url);
		$objuri = preg_replace("/page\/resource/", "resource", $url);
		$rdfuri = preg_replace("/page\/resource/", "data/resource", $url);
		$rdfuri = $rdfuri."?output=xml&all=1";
		$parser = ARC2::getRDFParser();
		$parser->parse( $rdfuri );
		$index = $parser->getSimpleIndex();
		$pvals = $index[$objuri];

		$startTime = $pvals[$ontNS."overallStartTime"][0];
		$endTime = $pvals[$ontNS."overallEndTime"][0];
		$status = $pvals[$ontNS."hasStatus"][0];
		$efile = $pvals[$ontNS."hasOriginalLogFile"][0];
		$tpage = $pvals[$ontNS."correspondsToTemplate"][0];
		$esys = $pvals[$ontNS."executedInWorkflowSystem"];
		$timages = $pvals[$ontNS."hasTemplateDiagram"];
		$rimages = $pvals[$ontNS."hasExecutionDiagram"];
		#$tfile = $pvals[$ontNS."hasOriginalTemplate"][0];
		#$csys = $pvals[$ontNS."createdInWorkflowSystem"];

		set_time_limit (0);

		if($startTime)
			$data = WTFactsAPI::addPV($subject, $data, "WorkflowStartTime", "_num", strtotime($startTime), true);
		if($endTime)
			$data = WTFactsAPI::addPV($subject, $data, "WorkflowEndTime", "_num", strtotime($endTime), true);
		if($status)
			$data = WTFactsAPI::addPV($subject, $data, "WorkflowStatus", "_str", $status, true);
		if($efile)
			$data = WTFactsAPI::addPV($subject, $data, "WorkflowWingsExecution", "_uri", $efile, true);
		if($tpage)
			$data = WTFactsAPI::addPV($subject, $data, "WorkflowTemplate", "_uri", $tpage, true);
		if($esys)
			foreach ($esys as $es)
				$data = WTFactsAPI::addPV($subject, $data, "WorkflowExecutedIn", "_uri", $es, true);
		if($timages)
			foreach ($timages as $timage)
				$data = WTFactsAPI::addPV($subject, $data, "WorkflowTemplateImage", "_uri", $timage, true);
		if($rimages)
			foreach ($rimages as $rimage)
				$data = WTFactsAPI::addPV($subject, $data, "WorkflowRunImage", "_uri", $rimage, true);
		#if($tfile)
		#	$data = WTFactsAPI::addPV($subject, $data, "WorkflowWingsTemplate", "_uri", $tfile);
		#if($csys)
		#	foreach ($csys as $cs)
		#		$data = WTFactsAPI::addPV($subject, $data, "WorkflowCreatedIn", "_uri", $cs);

		$ip_datasets = array();
		$op_datasets = array();
		$params = array();
		$procs = array();

		foreach($index as $key=>$val) {
			//$accounts = $val["http://openprovenance.org/model/opmo#account"];
			//if($accounts && in_array($objuri, $accounts)) {
				if(!array_key_exists("http://www.w3.org/1999/02/22-rdf-syntax-ns#type", $val))
					continue;
				$types = $val["http://www.w3.org/1999/02/22-rdf-syntax-ns#type"];
				if(!$types) continue;
				if(in_array($ontNS."WorkflowExecutionArtifact", $types)) {
					if(array_key_exists($ontNS."hasLocation", $val)) {
						if(array_key_exists("http://purl.org/net/opmv/ns#wasGeneratedBy", $val)) {
							$op_datasets[] = $key;
							$name = $val[$ontNS."hasFileName"][0];;
							$data = WTFactsAPI::addPV($subject, $data, "WorkflowDataArtifact", "_wpg", $name, true);
						} else {
							$ip_datasets[] = $key;
							$name = $val[$ontNS."hasFileName"][0];;
							$data = WTFactsAPI::addPV($subject, $data, "WorkflowInputDataArtifact", "_wpg", $name, true);
						}
					} else{
						$params[] = $key;
						$data = WTFactsAPI::addPV($subject, $data, "WorkflowParamArtifact", "_uri", $key, true);
					}
				}
				else if(in_array($ontNS."WorkflowExecutionProcess", $types)) {
					$procs[] = $key;
					$data = WTFactsAPI::addPV($subject, $data, "WorkflowProcess", "_uri", $key, true);
					//$name = preg_replace("/.+(#|\/)/", "", $key);
					//$data = WTFactsAPI::addPV($subject, $data, "WorkflowProcess", "_wpg", $name, true);
				}
				else if(in_array("http://purl.org/net/opmv/ns#Agent", $types)) {
					$data = WTFactsAPI::addPV($subject, $data, "WorkflowUser", "_uri", $key, true);
				}
			//}
		}
		$datasets = array_merge($ip_datasets, $op_datasets);
		$dsmapping = WTFactsAPI::createDatasetPages($datasets, $index);
		//print_r($procs);
		//print_r($datasets);
		//WTFactsAPI::createProcessPages($procs, $index, $dsmapping);

		return $data;
	}

	private static function createDatasetPages($datasets, $index) {
		global $ontNS;
		$dsmapping = array();

		$import_blacklist = array(
			"http://openprovenance.org/model/opmo#account",
			"http://www.w3.org/2000/01/rdf-schema#label"
		);
		foreach($datasets as $dset) {
			$info = $index[$dset];
			$dsname = preg_replace("/.+(#|\/)/", "", $dset);
			$dsfile = $dsname;

			$pvals = array();
			$varusage = null;
			foreach($info as $purl=>$vals) {
				if($purl == "http://www.w3.org/1999/02/22-rdf-syntax-ns#type") {
					foreach($vals as $val) {
						$vname = preg_replace("/.+(#|\/)/", "", $val);
						if($vname != "WorkflowExecutionArtifact" && $vname != "Artifact")
							$pvals[] = array("prop"=>"DataHasType", "value"=>$vname);
					}
				}
				else if($purl == $ontNS."hasLocation")
					$pvals[] = array("prop"=>"DataHasLocation", "value"=>$vals[0]);
				else if($purl == $ontNS."correspondsToTemplateArtifact") {
					foreach($vals as $val) {
						$val = preg_replace('/.+\//', '', $val);
						$varusage = $val;
						$pvals[] = array("prop"=>"DataUsedForVariable", "value"=>$val);
					}
				}
				else if($purl == $ontNS."hasFileName")
					$dsfile = preg_replace("/.+(#|\/)/", "", $vals[0]);
				else if(strpos($purl, "wasGeneratedByAs_") !== FALSE)
					continue;
				else if(!in_array($purl, $import_blacklist)) {
					$pname = preg_replace("/.+(#|\/)/", "", $purl);
					foreach($vals as $val)
						$pvals[] = array("prop"=>$pname, "value"=>$val);
				}
			}
			//$pvals[] = array("prop"=>"DataHasPubbyLink", "value"=>$dset);

			// Create Dataset Page
			$dstitle = Title::newFromText($dsfile);
			if(!$dstitle->exists()) {
				$text = "[[Category:Data]]\n";
				$summary = "Creating new Dataset";
				WTFactsAPI::createWikiPage($dsfile, $text, $summary);
				WTFactsAPI::addFactsMultiple($dsfile, $pvals, true);
			}
			else {
				if($varusage) WTFactsAPI::addFact($dsfile, "DataUsedForVariable", $varusage);
				//WTFactsAPI::addFact($dsfile, "DataHasPubbyLink", $dset);
			}

			$dsmapping[$dsname] = $dsfile;
			/*print "\n$dsfile\n";
			foreach($pvals as $pval) {
				print "\t".$pval["prop"]." = ".$pval["value"]."\n";
			}*/
		}
		return $dsmapping;
	}

	// Remove datasets who don't have any page with the property pointing to them
	private static function removeOrphanDatasetPages($datasets, $propDI) {
		$store = &smwfGetStore();
		foreach($datasets as $dset) {
			$dstitle = Title::newFromText($dset);
			$dsitem = SMWDIWikiPage::newFromTitle( $dstitle );
			$subjs = $store->getPropertySubjects($propDI, $dsitem);
			if(!sizeof($subjs)) {
				// Remove Dataset Page if no pages point to it
				WTFactsAPI::deleteWikiPage($dset, "Removing dataset as the workflow pointing to it was removed");
			}
		}
	}

	private static function createProcessPages($procs, $index, $dsmapping) {
		$import_blacklist = array(
			"http://www.w3.org/1999/02/22-rdf-syntax-ns#type",
			"http://openprovenance.org/model/opmo#account"
		);
		foreach($procs as $proc) {
			$info = $index[$proc];
			$procname = preg_replace("/.+(#|\/)/", "", $proc);
			$proclabel = "";

			$pvals = array();
			foreach($info as $purl=>$vals) {
				if($purl == "http://purl.org/net/opmv/ns#used") {
					foreach($vals as $val) {
						$vname = preg_replace("/.+(#|\/)/", "", $val);
						$dsname = array_key_exists($vname, $dsmapping) ? $dsmapping[$vname] : $vname;
						$pvals[] = array("prop"=>"usedDataset", "value"=>$dsname);
					}
				}
				else if($purl == "http://www.w3.org/2000/01/rdf-schema#label")
					$proclabel = $vals[0];
				else if(strpos($purl, "ontology/used_") !== FALSE)
					continue;
				else if(!in_array($purl, $import_blacklist)) {
					$pname = preg_replace("/.+(#|\/)/", "", $purl);
					foreach($vals as $val)
						$pvals[] = array("prop"=>$pname, "value"=>$val);
				}
			}
			print "\n$procname\n";
			print "/* $proclabel */\n";
			foreach($pvals as $pval) {
				print "\t".$pval["prop"]." = ".$pval["value"]."\n";
				//$proctitle = Title::newFromText($procname);
				//print $proctitle->exists()."\n";
			}
		}
	}

	private static function addPV($subject, $data, $prop, $ptype, $val, $create_prop=false) {
		$ptitle = Title::newFromText("Property:".$prop);
		if(!$ptitle->exists() && $create_prop) {
			$type = WTFactsAPI::getPropertyTypeFromDIType($ptype);
			$text = "Property type: [[has type::$type]]\n";
			$summary = "Creating new Property";
			WTFactsAPI::createWikiPage("Property:$prop", $text, $summary);
		}
		$propDI = SMWDIProperty::newFromUserLabel( $prop );
		$valDV = SMWDataValueFactory::newTypeIdValue ($ptype, $val);
		$data = WTFactsAPI::addPropValToData($data, $propDI, $valDV->getDataItem());
		return $data;
	}

	public static function getPropertyTypeFromDIType($type) {
		if($type == "_num")
			return "Number";
		if($type == "_bool")
			return "Boolean";
		if($type == "_uri")
			return "URL";
		if($type == "_str")
			return "String";
		if($type == "_txt")
			return "Text";
		if($type == "_wpg")
			return "Page";
		return "Page";
	}

	public static function guessValueType($value) {
		if(preg_match('/^-?\d+$/', $value))
			return "Number";
		if(preg_match('/^-?\d+\.\d+$/', $value))
			return "Number";
		if(preg_match('/^[2][0-9]{3}[-][0-1][0-9][-][0-3][0-9]/', $value))
			return "Date";
		if(preg_match('/^(true|false)$/i', $value))
			return "Boolean";
		if(preg_match("/^http/", $value))
			return "URL";
		if(preg_match("/^.+@.+\..+/", $value))
			return "Email";
		if(preg_match('/^".+"$/', $value))
			return "String";
		if(preg_match("/^'.+'$/", $value))
			return "String";
		return "Page";
	}

	protected function getAllowedParams() {
		return array (
			'operation' => null,
			'title' => null,
			'categories' => null,
			'property' => null,
			'value' => null,
			'totitle' => null,
			'oldvalue' => null,
			'subobjectsjson' => null,
			'type' => null
		);
	}

	protected function getParamDescription() {
		return array (
			'operation' => 'Operation to be performed. One of show, add, del, set, replace, newpage, updatesubobjects',
			'title' => 'Title of the page (required)',
			'property' => 'Property of the page (required for add, del)',
			'value' => 'Value of the page Property (required for add)',
			'totitle' => 'New Title of the page (required for move)',
			'oldvalue' => 'Old Value of the page Property (optional for set)',
			'subobjectsjson' => 'Json encoded Subobjects information to add '.
								'[{id:<subobjname>, properties:[{<propname>:<propval>}..] ]',
			'type' => 'Type of the Property value. One of page, number, boolean (optional for add)',
			'categories' => 'Categories of the new page (Separate with |)'
		);
	}

	protected function getDescription() {
		return 'Shows, adds or removes semantic facts for a page';
	}

	protected function getExamples() {
		return array (
			'api.php?action=wtfacts&operation=show&title=TestSMW&format=json'
		);
	}

	public function getVersion() {
		return __CLASS__ . ': $Id$';
	}
}

// Event handlers
WTFactsAPI::addEventHandler("Subscribes_To", WTFactsAPI::E_ADD, 'WTWorkingGroup::addWorkingGroupToWatchlist');
WTFactsAPI::addEventHandler("Subscribes_To", WTFactsAPI::E_DELETE, 'WTWorkingGroup::removeWorkingGroupFromWatchlist');
