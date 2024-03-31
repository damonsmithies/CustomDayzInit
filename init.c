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

	// Debug on/off
	const bool _debug = true;

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

	override void StartingEquipSetup(PlayerBase player, bool clothesChosen)
	{
		// Start By removing all default items.
		player.RemoveAllItems();

		// Get random clothes.
		getRandomClothes(player);
	
		// For debug suicide.
		player.GetInventory().CreateInInventory("HuntingKnife");
	}
};

Mission CreateCustomMission(string path)
{
	return new CustomMission();
};