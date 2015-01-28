<?php
require_once '../visualization/AlertIcon.php';
require_once '../visualization/NewIcon.php';
require_once '../visualization/ViewIcon.php';
require_once '../visualization/TaskIcon.php';
require_once '../visualization/OwnerIcon.php';
require_once '../visualization/TypeIcon.php';
require_once '../visualization/StartIcon.php';
require_once '../visualization/TargetIcon.php';
require_once '../visualization/ProgressIcon.php';
require_once '../visualization/ExpertiseIcon.php';
require_once '../visualization/ParticipantIcon.php';
require_once '../visualization/LockIcon.php';
require_once '../visualization/DelIcon.php';
require_once '../visualization/PersonIcon.php';
require_once '../visualization/PersonOwnerIcon.php';

class WTIconAPI{	
	
	public static function route($icon){		
		switch ($icon){
			case('alert'): self::alertIcon(); break;
			case('new'): self::newIcon(); break;
			case('view'): self::viewIcon(); break;
			case('task'): self::taskIcon(); break;
			case('owner'): self::ownerIcon(); break;
			case('type'): self::typeIcon(); break;
			case('start'): self::startIcon(); break;
			case('target'): self::targetIcon(); break;
			case('progress'): self::progressIcon(); break;
			case('expertise'): self::expertiseIcon(); break;
			case('participants'): self::participantIcon(); break;
			case('lock'): self::lockIcon(); break;
			case('del'): self::delIcon(); break;
			case('person'): self::personIcon(); break;
			case('personowner'): self::personOwnerIcon(); break;
			default: throw new IllegalValueException("Icon type not exist!", $icon);
		}
	}

	private static function alertIcon(){		
    	$i = new AlertIcon($_GET["alert"] == 'true');
    	$i->render();		
	}
	
	private static function newIcon(){		
    	$i = new NewIcon();
    	$i->render();		
	}
	
	private static function viewIcon(){		
    	$i = new ViewIcon($_GET["type"]);
    	$i->render();	
	}
	
	private static function taskIcon(){	
		$meta = $_GET["meta"] == 'true';
		$progress = $_GET["progress"]/100;
		$size = $_GET["size"];	
		$fade = $_GET["fade"] == 'true';
		$warning = $_GET["warning"] == 'true';	
		$childWarning = $_GET["childwarning"] == 'true';	
		
		$i = new TaskIcon($progress, $size, $fade, $warning, $childWarning);
		if($meta)
			$i->metaprogress();
		else{
			$type = $_GET["type"];	
			$overdue = $_GET["overdue"] == 'true';
			$childoverdue = $_GET["childoverdue"] == 'true';
			$i->progress($type, $overdue, $childoverdue);
		}
	    $i->render();
	}
	
	private static function ownerIcon(){
		$fade = $_GET['fade'] == 'true';
		$i = new OwnerIcon($fade);
		$i->render();
	}
	
	private static function typeIcon(){
		$fade = $_GET['fade'] == 'true';
		$warning = $_GET['warning'] == 'true';
		$i = new TypeIcon($fade, $warning);
		$i->render();
	}
	
	private static function startIcon(){
		$fade = $_GET['fade'] == 'true';
		$warning = $_GET['warning'] == 'true';
		$i = new StartIcon($fade, $warning);
		$i->render();
	}
	
	private static function targetIcon(){
		$fade = $_GET['fade'] == 'true';
		$warning = $_GET['warning'] == 'true';
		$i = new TargetIcon($fade, $warning);
		$i->render();
	}
	
	private static function progressIcon(){
		$fade = $_GET['fade'] == 'true';
		$i = new ProgressIcon($fade);
		$i->render();
	}
	
	private static function expertiseIcon(){
		$fade = $_GET['fade'] == 'true';
		$i = new ExpertiseIcon($fade);
		$i->render();
	}
	
	private static function participantIcon(){
		$fade = $_GET['fade'] == 'true';
		$i = new ParticipantIcon($fade);
		$i->render();
	}
	
	private static function lockIcon(){
		$i = new LockIcon();
		$i->render();
	}
	
	private static function delIcon(){
		$i = new DelIcon();
		$i->render();
	}
	
	private static function personIcon(){
		$i = new PersonIcon();
		$i->render();
	}
	
	private static function personOwnerIcon(){
		$isOwner = $_GET['isowner'] == 'true';
		$i = new PersonOwnerIcon($isOwner);
		$i->render();
	}
	

	
}


WTIconAPI::route($_GET["icon"]);




?>

