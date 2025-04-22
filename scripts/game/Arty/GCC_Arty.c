[EntityEditorProps()]
class GCC_ArtyClass : GenericEntityClass
{
}

class GCC_Arty : GenericEntity
{
	[Attribute(defvalue: "", UIWidgets.Object)]
    protected ref array<ref GCC_ArtyData> m_artyRounds;
	
	protected ref array<ref GCC_ArtilleryMission> missions = {};
	protected RplComponent m_RplComponent;
	protected static GCC_Arty m_instance;
	
	void CallMission(string faction, int roundIndex, int roundCount, int gunCount, int roundDispersion, vector position)
	{
		GCC_ArtyData round = FindRound(faction, roundIndex);
		if(!IsMissionVaild(round, roundCount, gunCount))
			return;
		
		roundCount = Math.ClampInt(roundCount, roundCount, round.m_roundCount);
		if(roundCount < 1 || roundCount > round.m_roundCount)
			return;
		
		UpdateRound(faction, roundIndex, round.m_roundCount - roundCount);
		
		GCC_ArtilleryMission mission = new GCC_ArtilleryMission();
		mission.m_round = round;
		mission.m_totalRounds = roundCount;
		mission.m_gunCount = gunCount;
		mission.m_desiredDispersion = roundDispersion;
		mission.m_targetPosition = position;
		mission.Start();
		
		missions.Insert(mission);
	}
	
	bool IsMissionVaild(GCC_ArtyData round, int roundCount, int gunCount)
	{
		if(!round)
			return false;
		
		if(m_RplComponent == RplRole.Proxy)
			return false;
		
		if(roundCount < 1)
			return false;
		
		if(gunCount < 1)
			return false;
		
		if(round.m_roundCount < 1)
			return false;
		
		return true;
	}
	
	void UpdateRound(string factionKey, int index, int roundCount)
	{
		Rpc(GCC_RpcDo_UpdateRound, factionKey, index, roundCount);
		GCC_RpcDo_UpdateRound(factionKey, index, roundCount);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void GCC_RpcDo_UpdateRound(string factionKey, int index, int roundCount)
	{
		GCC_ArtyData round = FindRound(factionKey, index);
		if(!round)
			return;
		
		round.m_roundCount = roundCount;
		
		MenuManager menuManager = GetGame().GetMenuManager();
		GCC_ArtyMenu menu = GCC_ArtyMenu.Cast(menuManager.FindMenuByPreset(ChimeraMenuPreset.GCC_ArtyMenu));
		if(!menu)
			return;
		
		menu.UpdateRound(null, index);
	}
	
	GCC_ArtyData FindRound(string factionKey, int index)
	{
		array<ref GCC_ArtyData> rounds = GetRoundsFaction(factionKey);
		if(rounds)
			return rounds[index];
		
		return null;
	}
	
	array<ref GCC_ArtyData> GetRoundsFaction(string factionKey)
	{
		array<ref GCC_ArtyData> outRounds = {};
		foreach(GCC_ArtyData round : m_artyRounds)
		{
			array<string> factions = round.m_factionKeys;
			if(factions.IsEmpty() || round.m_factionKeys.Contains(factionKey))
				outRounds.Insert(round);
		}

		return outRounds;
	}
	
	array<int> GetFrequenciesFaction(string factionKey)
	{
		array<int> outFreqs = {};
		array<ref GCC_ArtyData> rounds = GetRoundsFaction(factionKey);
		foreach(GCC_ArtyData round : rounds)
		{
			foreach(int freq : round.m_allowedFrequencies)
			{
				outFreqs.Insert(freq);
			}
		}
		
		return outFreqs;
	}

	bool IsActionValid()
	{
		SCR_ChimeraCharacter player = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerController().GetControlledEntity());
		if(!player)
			return false;
		
		if (!player.m_pFactionComponent)
			return false;

		string faction = player.m_pFactionComponent.GetAffiliatedFaction().GetFactionKey();
		array<int> allowedFreqs = GetFrequenciesFaction(faction);
		if (!faction)
			return false;
		
		array<ref GCC_ArtyData> rounds = GetRoundsFaction(faction);
		if(!rounds || rounds.IsEmpty())
			return false;
		
		//Frequencies
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if(!player)
			return false;

		array<IEntity> items = {};
		inventory.GetAllRootItems(items);
		foreach (IEntity item : items)
		{
			if(!item.FindComponent(SCR_RadioComponent))
				continue;

			SCR_RadioComponent radio = SCR_RadioComponent.Cast(item.FindComponent(SCR_RadioComponent));
			BaseRadioComponent radioBase = BaseRadioComponent.Cast(item.FindComponent(BaseRadioComponent));
			
			bool isFreqValid = false;
			if (!allowedFreqs.IsEmpty() && radioBase && radioBase.TransceiversCount() != 0)
			{
				for (int i = radioBase.TransceiversCount() - 1; i >= 0; --i)
				{
					int freq = radioBase.GetTransceiver(i).GetFrequency();
					if(allowedFreqs.Contains(freq))
					{
						isFreqValid = true;
						break;
					}
				}
			}
			
			if(isFreqValid || allowedFreqs.IsEmpty())
			{
				if(radio.GetType() == EGadgetType.RADIO_BACKPACK)
					return true;
				
				if(radio.GetMode() == EGadgetMode.IN_HAND)
					return true;
			}
		}
		
		return false;
	}
	
	void OpenAction()
	{
		if(!IsActionValid())
			return;
		
		GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.GCC_ArtyMenu);
	}
	
	static GCC_Arty GetInstance()
	{
		return m_instance;
	}
	
	override protected void EOnInit(IEntity owner)
	{
		m_instance = this;
		m_RplComponent = RplComponent.Cast(owner.FindComponent(RplComponent));
		
		GetGame().GetInputManager().AddActionListener("GCC_ArtyOpenMenu", EActionTrigger.DOWN, OpenAction);
	}
	
	void GCC_Arty(IEntitySource src, IEntity parent)
	{
		SetEventMask(EntityEvent.INIT);
	}
	
	override bool RplSave(ScriptBitWriter writer)
	{
		int artyRoundsCount = m_artyRounds.Count();
		writer.WriteInt(artyRoundsCount);
		for (int i = 0; i < artyRoundsCount; i++)
		{
			writer.WriteInt(m_artyRounds[0].m_roundCount);
		}
		
		return true;
	}
	
	override bool RplLoad(ScriptBitReader reader)
	{
		int artyRoundsCount;
		reader.ReadInt(artyRoundsCount);
		for (int i = 0; i < artyRoundsCount; i++)
		{
			int count;
			reader.ReadInt(count);

			m_artyRounds[0].m_roundCount = count;
		}
		
		return true;
	}
}

class GCC_ArtilleryMission
{
	GCC_ArtyData m_round;
	int m_desiredDispersion;
	int m_totalRounds;
	int m_gunCount
	vector m_targetPosition;
	protected int m_totalRoundsFired = 0;
	
	void Start()
	{
		FireBarrage();
	}

	void FireBarrage()
	{
		int roundsThisBarrage = Math.Min(m_gunCount, m_totalRounds - m_totalRoundsFired);

		if (roundsThisBarrage <= 0)
			return;

		for (int i = 0; i < roundsThisBarrage; i++)
		{
			FireRound();
			m_totalRoundsFired++;
		}
		
		if(m_totalRoundsFired < m_totalRounds)
			GetGame().GetCallqueue().CallLater(FireBarrage, m_round.m_roundReload * 1000, false);
	}

	void FireRound()
	{
		int dispersion = Math.Max(m_round.m_roundDispersion, m_desiredDispersion);
		
		vector offset = GetRandomOffset(dispersion);
		vector target = m_targetPosition + offset;
		
		float delay = m_round.m_roundDelay + Math.RandomFloat(0,5);
		GetGame().GetCallqueue().CallLater(m_round.Launch, delay * 1000, false, target);
	}

	vector GetRandomOffset(float dispersion)
	{
		float angle = Math.RandomFloat(0, Math.PI2);
		float rad = Math.RandomFloat(0, dispersion);

		return Vector(Math.Cos(angle) * rad, 0, Math.Sin(angle) * rad);
	}
}

[BaseContainerProps()]
class GCC_ArtyData
{
	[Attribute("{38BAE094333E31BF}Prefabs/Weapons/Ammo/Ammo_Shell_81mm_HE_M821.et", UIWidgets.ResourceNamePicker, "Shell prefab")]
	ResourceName m_shellPrefab;

	[Attribute("81mm HE M821", UIWidgets.Auto, "Round name")]
	string m_roundName;
	
	[Attribute("", UIWidgets.Auto, desc: "Factions that will be able to access rounds (Blank = all)")]
	ref array<string> m_factionKeys;
	
	[Attribute(defvalue: "", UIWidgets.Auto, desc: "Factions that will be able to access rounds (Blank = all)")]
    ref array<int> m_allowedFrequencies;
	
	[Attribute("16", UIWidgets.Auto, "Round count")]
	int m_roundCount;
	
	[Attribute("1", UIWidgets.Auto, "Gun count")]
	int m_gunCount;
	
	[Attribute("90", UIWidgets.Auto, "Round splash time (Seconds)")]
	float m_roundDelay;

	[Attribute("6", UIWidgets.Auto, "Round reload time (Seconds)")]
	float m_roundReload;
	
	[Attribute("30", UIWidgets.Auto, "Round dispersion (Radius in meters)")]
	float m_roundDispersion;
	
	void Launch(vector targetPosition)
	{
		EntitySpawnParams spawnParams = new EntitySpawnParams();
		spawnParams.Transform[3] = targetPosition + vector.Up * 100;
		IEntity shell = GetGame().SpawnEntityPrefab(Resource.Load(m_shellPrefab), null, spawnParams);
		
		ProjectileMoveComponent moveComponent = ProjectileMoveComponent.Cast(shell.FindComponent(ProjectileMoveComponent));
		if (!moveComponent)
			return;
		
		vector transform[4];
		shell.GetTransform(transform);
		
		SCR_Math3D.LookAt(transform[3], targetPosition, vector.Up, transform);
		shell.SetTransform(transform);
		
		moveComponent.Launch(vector.Up * -1, vector.Zero, 0.25, shell, null, null, null, null);
	}
}

[BaseContainerProps()]
class GCC_ArtyOpenMenuAvailable : SCR_AvailableActionCondition
{
	override bool IsAvailable(SCR_AvailableActionsConditionData data)
	{
		GCC_Arty arty = GCC_Arty.GetInstance();
		if(!arty)
			return false;
		
		return arty.IsActionValid();
	}
}
