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

class config
{

	// Debug on/off
	const bool _debug = true;

	// Global chance modifier. Use for randomizing items chances unless overridden for a specific random chance item. Treat as a %.
	const int max_chance = 5; // 5%

	// Random chance of spawning with gloves on/off.
	const bool random_chance_spawn_gloves = true;
	const int max_chance_override_gloves = 0;

	// Random chance of spawning with face covering on/off.
	const bool random_chance_spawn_face = true;
	const int max_chance_override_face = 0;

	// Random chance of spawning with head item on/off.
	const bool random_chance_spawn_head = true;
	const int max_chance_override_head = 0;

	// Random chance of spawning with bag on/off.
	const bool random_chance_spawn_bag = true;
	const int max_chance_override_bag = 0;

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

	// Dice roll function.
    static bool DiceRoll(int maxChance)
    {
		bool outcome;
        int roll = Math.RandomInt(0, 100);
		//bool outcome = (maxChance <= roll) ? true : false;
		if (maxChance <= roll)
		{
			outcome = true;
		}
		else
		{
			outcome = false;
		}
        return outcome;
    }

	// Set Character into game
	override PlayerBase CreateCharacter(PlayerIdentity identity, vector pos, ParamsReadContext ctx, string characterName)
	{
		Entity playerEnt = GetGame().CreatePlayer( identity, characterName, pos, 0, "NONE" );
		Class.CastTo(m_player, playerEnt);
		GetGame().SelectPlayer( identity, m_player );
		return m_player;
	}

	// https://github.com/ravmustang/DayZ_SA_ClassName_Dump/blob/master/Everything%20DayZ/DayZ%20SA%20Community%20Scripting/PVPLoadout_init.c
	private void getRandomClothes(PlayerBase player)
	{
		// Spawn random top on player.
		array<string> gear_top = {"Shirt_BlueCheck", "Shirt_BlueCheckBright", "Shirt_GreenCheck", "Shirt_PlaneBlack", "Shirt_RedCheck", "Shirt_WhiteCheck"}
		EntityAI player_top = player.GetInventory().CreateInInventory(gear_top.GetRandomElement());

		// Spawn random pants on player.
		array<string> gear_pants = {"Jeans_Black", "Jeans_Blue", "Jeans_BlueDark", "Jeans_Brown", "Jeans_Green", "Jeans_Grey"};
		EntityAI player_pants = player.GetInventory().CreateInInventory(gear_pants.GetRandomElement());

		// Spawn random shoes on player.
		array<string> gear_shoes = {"AthleticShoes_Black", "AthleticShoes_Green", "AthleticShoes_Blue", "AthleticShoes_Brown", "AthleticShoes_Grey"};
		EntityAI player_shoes = player.GetInventory().CreateInInventory(gear_shoes.GetRandomElement());
	}

	private void getChanceClothes(PlayerBase player)
	{
		int chanceGlobal = config.max_chance;
		if (config.random_chance_spawn_gloves)
		{
			// Set chance of gloves.
			int chanceGloves = config.max_chance_override_gloves

			// Override check for gloves.
			if(chanceGloves > 0)
			{
				int chanceComparitor = chanceGloves;
			}
			else
			{
				int chanceComparitor = chanceGlobal;
			}

			// Roll the dice for gloves and add to player on succesfull roll.
			bool gloves_roll = DiceRoll(chanceComparitor);
			if(gloves_roll)
			{
				// Spawn random gloves on player.
				array<string> gear_gloves = {"WorkingGloves_Black", "WorkingGloves_Brown", "WorkingGloves_Beige" , "WorkingGloves_Yellow"};
				EntityAI player_gloves = player.GetInventory().CreateInInventory(gear_gloves.GetRandomElement());
			}
		}
		if (config.random_chance_spawn_face)
		{
			// Set chance of face.
			int chanceFace = config.max_chance_override_face

			// Override check for face.
			if(chanceFace > 0)
			{
				int chanceComparitor = chanceFace;
			}
			else
			{
				int chanceComparitor = chanceGlobal;
			}

			// Roll the dice for face and add to player on succesfull roll.
			bool face_roll = DiceRoll(chanceComparitor);
			if(face_roll)
			{
				// Spawn random face item on player.
				array<string> gear_face = {"BandanaMask_BlackPattern", "BandanaMask_CamoPattern", "BandanaMask_GreenPattern", "BandanaMask_PolkaPattern", "BandanaMask_RedPattern"};
				EntityAI player_face = player.GetInventory().CreateInInventory(gear_face.GetRandomElement());
			}
		}
		if (config.random_chance_spawn_head)
		{
			// Set chance of head.
			int chanceHead = config.max_chance_override_head

			// Override check for head.
			if(chanceHead > 0)
			{
				int chanceComparitor = chanceHead;
			}
			else
			{
				int chanceComparitor = chanceGlobal;
			}

			// Roll the dice for head and add to player on succesfull roll.
			bool head_roll = DiceRoll(chanceComparitor);
			if(head_roll)
			{
				// Spawn random head item on player.
				array<string> gear_head = {"Bandana_Blackpattern", "Bandana_Camopattern", "Bandana_Greenpattern", "Bandana_Polkapattern", "Bandana_Redpattern"};
				EntityAI player_head = player.GetInventory().CreateInInventory(gear_head.GetRandomElement());
			}
		}
		if (config.random_chance_spawn_bag)
		{
			// Set chance of bag.
			int chanceBag = config.max_chance_override_bag

			// Override check for gloves.
			if(chanceBag > 0)
			{
				int chanceComparitor = chanceBag;
			}
			else
			{
				int chanceComparitor = chanceGlobal;
			}

			// Roll the dice for bag and add to player on succesfull roll.
			bool bag_roll = DiceRoll(chanceComparitor);
			if(bag_roll)
			{
				// Spawn random bag item on player.
				array<string> gear_bag =  {"CanvasBag_Medical", "CanvasBag_Olive", "DuffelBagSmall_Camo", "DuffelBagSmall_Medical", "Slingbag_Black", "Slingbag_Brown", "Slingbag_Gray"};
				EntityAI player_bag = player.GetInventory().CreateInInventory(gear_bag.GetRandomElement());
			}
		}
	}

	override void StartingEquipSetup(PlayerBase player, bool clothesChosen)
	{
		// Start By removing all default items.
		player.RemoveAllItems();

		// Get random clothes.
		getRandomClothes(player);

		// Get chance clothes.
		getChanceClothes(player);
	
		// For debug suicide.
		if (config._debug)
		{
			player.GetInventory().CreateInInventory("HuntingKnife");
		}
	}
};

Mission CreateCustomMission(string path)
{
	return new CustomMission();
};