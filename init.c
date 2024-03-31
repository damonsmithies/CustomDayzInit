static void SpawnObject(string type, vector position, vector orientation)
{
    auto obj = GetGame().CreateObjectEx(type, position, ECE_SETUP | ECE_UPDATEPATHGRAPH | ECE_CREATEPHYSICS);
    obj.SetPosition(position);
    obj.SetOrientation(orientation);
    obj.SetOrientation(obj.GetOrientation());
    obj.SetFlags(EntityFlags.STATIC, false);
    obj.Update();
	obj.SetAffectPathgraph(true, false);
	if (obj.CanAffectPathgraph()) GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(GetGame().UpdatePathgraphRegionByObject, 100, false, obj);
};

void main()
{
	//INIT WEATHER BEFORE ECONOMY INIT------------------------
	Weather weather = g_Game.GetWeather();

	weather.MissionWeather(false);    // false = use weather controller from Weather.c

	weather.GetOvercast().Set( Math.RandomFloatInclusive(0.4, 0.6), 1, 0);
	weather.GetRain().Set( 0, 0, 1);
	weather.GetFog().Set( Math.RandomFloatInclusive(0.05, 0.1), 1, 0);

	//INIT ECONOMY--------------------------------------
	Hive ce = CreateHive();
	if ( ce )
		ce.InitOffline();

	//DATE RESET AFTER ECONOMY INIT-------------------------
	int year, month, day, hour, minute;
	int reset_month = 9, reset_day = 20;
	GetGame().GetWorld().GetDate(year, month, day, hour, minute);

	if ((month == reset_month) && (day < reset_day))
	{
		GetGame().GetWorld().SetDate(year, reset_month, reset_day, hour, minute);
	}
	else
	{
		if ((month == reset_month + 1) && (day > reset_day))
		{
			GetGame().GetWorld().SetDate(year, reset_month, reset_day, hour, minute);
		}
		else
		{
			if ((month < reset_month) || (month > reset_month + 1))
			{
				GetGame().GetWorld().SetDate(year, reset_month, reset_day, hour, minute);
			}
		}
	}
};

// Add your BuilderItems or DayZ Editor Code Here for building and item placement

class CONFIG
{

	// Randomise spawn gear on/off.
	const bool random_spawn_gear = true;

	// Random chance of spawning with extra gear on/off.
	const bool random_chance_spawn_gear = true;

	// Random chance of spawning with extra items on/off.
	const bool random_chance_spawn_items = true;

	// Random chance of spawning with extra food or drink on/off.
	const bool random_chance_spawn_food_drink = true;

	// Random chance of spawning with extra medical on/off.
	const bool random_chance_spawn_medical = true;

	// Global chance modifier. Use for randomizing items chances. Treat as a %.
	const int max_rand = 5; // 5%

	// Global chance modifier for food and drink.
	const int food_chance = 5; // 5%
	const int drink_chance = 5; // 5%

	// Global item health modifier. Use for randomising spawned item health.
	const int min_item_health = 4; // Badly Damaged
	const int max_item_health = 1; // Pristine

	// Set how many bandages or rags player starts with.
	const int min_medical_item = 2;
	const int max_medical_item = 6;

};

class CustomMission: MissionServer
{

	// Namalsk specific
	override void OnInit()
	{
		super.OnInit();

		// this piece of code is recommended otherwise event system is switched on automatically and runs from default values
		// comment this whole block if NOT using Namalsk Survival
		if ( m_EventManagerServer )
		{
			// enable/disable event system, min time between events, max time between events, max number of events at the same time
			m_EventManagerServer.OnInitServer( true, 550, 1000, 2 );
			// registering events and their probability
			m_EventManagerServer.RegisterEvent( Aurora, 0.85 );
			m_EventManagerServer.RegisterEvent( Blizzard, 0.4 );
			m_EventManagerServer.RegisterEvent( ExtremeCold, 0.4 );
			m_EventManagerServer.RegisterEvent( Snowfall, 0.6 );
			m_EventManagerServer.RegisterEvent( EVRStorm, 0.35 );
			m_EventManagerServer.RegisterEvent( HeavyFog, 0.3 );
		}
	}

	// Set Character into game
	override PlayerBase CreateCharacter(PlayerIdentity identity, vector pos, ParamsReadContext ctx, string characterName)
	{
		Entity playerEnt = GetGame().CreatePlayer( identity, characterName, pos, 0, "NONE" );
		Class.CastTo(m_player, playerEnt);
		GetGame().SelectPlayer( identity, m_player );
		return m_player;
	}

	override void StartingEquipSetup(PlayerBase player, bool clothesChosen)
	{
		// Start By removing all default items.
		player.RemoveAllItems(); 

		player.GetInventory().CreateInInventory("TTSKOPants");
		player.GetInventory().CreateInInventory("TTsKOJacket_Camo");
		player.GetInventory().CreateInInventory("CombatBoots_Black");
		player.GetInventory().CreateInInventory("ImprovisedBag");

		player.GetInventory().CreateInInventory("SodaCan_Pipsi");
		player.GetInventory().CreateInInventory("SpaghettiCan");
		player.GetInventory().CreateInInventory("HuntingKnife");
		ItemBase rags = player.GetInventory().CreateInInventory("Rag");
		rags.SetQuantity(4);

		EntityAI primary;
		EntityAI axe = player.GetInventory().CreateInInventory("FirefighterAxe");

		player.SetQuickBarEntityShortcut(rags, 2, true);
		player.SetQuickBarEntityShortcut(axe, 3, true);
	}
};

Mission CreateCustomMission(string path)
{
	return new CustomMission();
};
/*

	void SendPlayerMessage(PlayerBase player, string message)	
	{
		Param1<string> Msgparam;
		Msgparam = new Param1<string>(message);
		GetGame().RPCSingleParam(player, ERPCs.RPC_USER_ACTION_MESSAGE, Msgparam, true, player.GetIdentity());
	}

	// Global Variables
	EntityAI itemClothing;
	EntityAI itemTop;
	EntityAI itemEnt, Char_Bag, Char_Gloves, Char_Top, Char_Pants, Char_Shoes, Char_Chemlight, Char_Melee, attachment;
	ItemBase itemBs;

	// Convert variables to string
	string RandomMelee;
	string RandomBag;
	string RandomChemlight;

	// Function to set randomised clothing/player quality.
	void SetRandomHealth(EntityAI itemEnt)
	{
		if (itemEnt)
		{
			float rndHlt = Math.RandomFloat(CONFIG.min_item_health, CONFIG.max_item_health);
			itemEnt.SetHealth01( "", "", rndHlt );
		}
	}

	override void StartingEquipSetup(PlayerBase player, bool clothesChosen)
	{	
		


		// If random_spawn_gear set to true, construct the arrays.
		if (CONFIG.random_spawn_gear)
		{
			// Randomised spawn clothing arrays.
			static ref const array<string> GEAR_TOP = {"Shirt_BlueCheck", "Shirt_BlueCheckBright", "Shirt_GreenCheck", "Shirt_PlaneBlack", "Shirt_RedCheck", "Shirt_WhiteCheck"}
			static ref const array<string> GEAR_PANTS = {"Jeans_Black", "Jeans_Blue", "Jeans_BlueDark", "Jeans_Brown", "Jeans_Green", "Jeans_Grey"};
			static ref const array<string> GEAR_SHOES = {"AthleticShoes_Black", "AthleticShoes_Green", "AthleticShoes_Blue", "AthleticShoes_Brown", "AthleticShoes_Grey"};
		}

		/*
		// If random_spawn_gear set to true, construct the arrays.
		if (CONFIG.random_chance_spawn_gear)
		{
			static ref const array<string> GEAR_GLOVES = {"WorkingGloves_Black", "WorkingGloves_Brown", "WorkingGloves_Beige" , "WorkingGloves_Yellow"};
			static ref const array<string> GEAR_FACE = {"BandanaMask_BlackPattern", "BandanaMask_CamoPattern", "BandanaMask_GreenPattern". "BandanaMask_PolkaPattern", "BandanaMask_RedPattern"};
			static ref const array<string> GEAR_HEAD = {"Bandana_Blackpattern", "Bandana_Camopattern", "Bandana_Greenpattern", "Bandana_Polkapattern", "Bandana_Redpattern"};
			static ref const array<string> GEAR_BACKPACK = {"CanvasBag_Medical", "CanvasBag_Olive", "DuffelBagSmall_Camo", "DuffelBagSmall_Medical", "Slingbag_Black", "Slingbag_Brown", "Slingbag_Gray"};
		}

		// If random_spawn_items set to true, construct the arrays.
		if (CONFIG.random_chance_spawn_items)
		{
			static ref const array<string> ITEM_CHEMLIGHT = {"Chemlight_White", "Chemlight_Yellow", "Chemlight_Green", "Chemlight_Red"};
			static ref const array<string> ITEM_KNIFE = {"StoneKnife", "KitchenKnife", "SteakKnife","BoneKnife", "HuntingKnife"};
		}

		// If random_spawn_food_drink set to true, construct the arrays.
		if (CONFIG.random_chance_spawn_food_drink)
		{
			static ref const array<string> ITEM_FOOD = {"PeachesCan", "SpaghettiCan", "BakedBeansCan"};
			static ref const array<string> ITEM_DRINK = {"SodaCan_Cola", "SodaCan_Pipsi", "SodaCan_Spite", "SodaCan_Kvass"};
		}
		

		// If random_spawn_medical set to true, construct the arrays.
		if (CONFIG.random_chance_spawn_medical)
		{
			static ref const array<string> ITEM_MEDICAL = {"Bandage", "BandageDressing"};
		}


		// Set randomised base clothing loadout if random_spawn_gear set to TRUE in config.
		if (CONFIG.random_spawn_gear)
		{
			string top_random = GEAR_TOP.GetRandomElement()
			string pants_random = GEAR_PANTS.GetRandomElement()
			string shoes_random = GEAR_SHOES.GetRandomElement()
			//Char_Top = player.GetInventory().CreateInInventory(CONFIG.GEAR_TOP.GetRandomElement());

			itemClothing = player.FindAttachmentBySlotName( "Body" );
			if ( itemClothing )
			{
				Char_Top = itemClothing.GetInventory().CreateInInventory(top_random);
				//Char_Pants = player.GetInventory().CreateInInventory(CONFIG.GEAR_PANTS.GetRandomElement());
			}

			Char_Pants = player.GetInventory().CreateInInventory(pants_random);
			//Char_Shoes = player.GetInventory().CreateInInventory(CONFIG.GEAR_SHOES.GetRandomElement());
			Char_Shoes = player.GetInventory().CreateInInventory(shoes_random);
			SendPlayerMessage(player, "DEBUG MESSAGE: Top selected: " + top_random + " | Pants selected: " + pants_random + " | Shoes selected: " + shoes_random);
		}
	};
	
	/*
	protected void sendPlayerMessage(PlayerBase player, string message)    
    {
		if((player) && (message != ""))
		{
			Param1<string> Msgparam;
			Msgparam = new Param1<string>(message);
			GetGame().RPCSingleParam(player, ERPCs.RPC_USER_ACTION_MESSAGE, Msgparam, true, player.GetIdentity());
            GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(SurvivorDetected);
		}
    }
	*/
