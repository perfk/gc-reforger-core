class GC_MarkerAreaClass: ScriptComponentClass
{
}

class GC_MarkerArea : ScriptComponent
{
	[Attribute("100", UIWidgets.Auto, "Initial polygon radius in meters. This is the distance from the marker center to each polygon vertex.", category: "Visual")]
	protected float m_fBaseRadiusMeters;

	[Attribute("50", UIWidgets.Auto, "Number of polygon sides. Minimum valid value is 3.", category: "Visual")]
	protected int m_iPolygonSides;
	
	[Attribute(defvalue: "0", uiwidget: UIWidgets.Slider, params: "0 360 1", desc: "Polygon rotation offset in degrees.", category: "Visual")]
	protected float m_fRotationOffsetDegrees;
	
	[Attribute("0 0 0 0.5", UIWidgets.Auto, "Fill color of the marker polygon.", category: "Visual")]
	protected ref Color m_MarkerColor;
	
	[Attribute("", UIWidgets.Auto, desc: "Faction keys that can see this marker. Leave empty to show the marker to all factions.", category: "Visibility")]
	protected ref array<string> m_aVisibleFactionKeys;
	
	[Attribute("0", UIWidgets.Auto, "Time in seconds between marker updates. Higher values update less often.", params: "0 inf", category: "Update")]
	protected float m_fUpdateIntervalSeconds;
	
	[Attribute("0", UIWidgets.Auto, "Radius change in meters per update. Positive values expand the marker. Negative values shrink it. 0 disables radius changes.", category: "Update")]
	protected float m_fRadiusChangePerUpdate;
	
	[Attribute("0", UIWidgets.Auto, "Maximum total radius change from the initial radius. 0 means unlimited. The marker radius will never go below 1 meter.", category: "Update")]
	protected float m_fMaxRadiusChangeMeters;

	[Attribute("", UIWidgets.Auto, "Text displayed next to the marker. Leave empty to hide marker text.", category: "Text")]
	protected string m_sMarkerText;
	
	[Attribute("1 1 1 1", UIWidgets.Auto, "Color of the marker text.", category: "Text")]
	protected ref Color m_MarkerTextColor;

	protected SCR_MapEntity m_MapEntity;
	protected CanvasWidget m_wCanvasWidget;
	protected FrameWidget m_wTextFrame;
	
	protected ref array<ref CanvasWidgetCommand> m_aDrawCommands = {};
	protected vector m_vMarkerCenter;
	protected float m_fCurrentRadiusMeters;
	
	override protected void OnPostInit(IEntity owner)
	{
		m_fCurrentRadiusMeters = m_fBaseRadiusMeters;
		m_MapEntity = SCR_MapEntity.GetMapInstance();
		
		if (!m_MapEntity)
			return;

		m_MapEntity.GetOnMapOpen().Insert(OnMapOpen);
		m_MapEntity.GetOnMapClose().Insert(OnMapClose);
		m_MapEntity.GetOnMapPan().Insert(OnMapPan);
		
		RplComponent rpl = RplComponent.Cast(owner.FindComponent(RplComponent));
		if (rpl && rpl.Role() == RplRole.Authority)
			Start();
		
		if(m_MapEntity.IsOpen())
			Setup();
	}
	
	protected void OnMapOpen(MapConfiguration mapConfig)
	{
		Setup();
	}
	
	protected void Setup()
	{
		if (!m_wCanvasWidget)
		{
			Widget canvasWidget = GetGame().GetWorkspace().CreateWidgets("{AC73DC5AE6BEF564}UI/layouts/Map/GC_Markers.layout", m_MapEntity.GetMapMenuRoot());
			m_wCanvasWidget = CanvasWidget.Cast(canvasWidget);
		}

		if (!m_wTextFrame)
		{
			Widget textWidget = GetGame().GetWorkspace().CreateWidgets("{9E3AE523FF814055}UI/layouts/Map/GC_MarkerText.layout", m_MapEntity.GetMapMenuRoot());
			m_wTextFrame = FrameWidget.Cast(textWidget);

			RichTextWidget richText = RichTextWidget.Cast(m_wTextFrame.FindAnyWidget("text"));
			richText.SetText(m_sMarkerText);
			richText.SetColor(m_MarkerTextColor);
		}

		DrawShape();
	}
	
	protected void OnMapClose(MapConfiguration mapConfig)
	{
		m_aDrawCommands.Clear();
	}
	
	protected void OnMapPan(float x, float y, bool adjustedPan)
	{
		DrawShape();
	}
	
	protected void DrawShape()
	{
		if (!m_vMarkerCenter || !m_fCurrentRadiusMeters || !m_wCanvasWidget)
			return;

		if (!IsMarkerVisible())
			return;

		array<vector> worldVertices = {};
		array<float> screenVertices = GetShape(m_vMarkerCenter, m_fCurrentRadiusMeters, m_iPolygonSides, worldVertices);
		
		m_aDrawCommands.Clear();
		
		PolygonDrawCommand drawCommand = new PolygonDrawCommand();
		drawCommand.m_iColor = m_MarkerColor.PackToInt();
		drawCommand.m_Vertices = screenVertices;
		
		m_aDrawCommands.Insert(drawCommand);
		m_wCanvasWidget.SetDrawCommands(m_aDrawCommands);
		
		UpdateText(worldVertices);
	}
	
	protected bool IsMarkerVisible()
	{
		if (m_aVisibleFactionKeys.IsEmpty())
			return true;
		
		SCR_PlayerController pc = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		if (!pc)
			return false;
		
		SCR_PlayerFactionAffiliationComponent affiliation = SCR_PlayerFactionAffiliationComponent.Cast(pc.FindComponent(SCR_PlayerFactionAffiliationComponent));
		if (!affiliation)
			return false;
		
		Faction faction = affiliation.GetAffiliatedFaction();
		if (!faction)
			return false;
		
		FactionKey fkey = faction.GetFactionKey();
		
		return (m_aVisibleFactionKeys.Contains(fkey));
	}
	
	protected array<float> GetShape(vector centerPosition, float radiusMeters, int sides, out array<vector> worldVertices)
	{
		array<float> screenVertices = {};
		worldVertices = GetShapeWorld(centerPosition, radiusMeters, sides);
	
		float duplicateVertexTolerance = 0.01;
	
		foreach (vector worldPoint : worldVertices)
		{
			float screenX;
			float screenY;
	
			m_MapEntity.WorldToScreen(worldPoint[0], worldPoint[2], screenX, screenY, true);
	
			bool vertexAlreadyExists = false;
	
			for (int i = 0; i < screenVertices.Count(); i += 2)
			{
				float existingX = screenVertices[i];
				float existingY = screenVertices[i + 1];
	
				if (Math.AbsFloat(existingX - screenX) <= duplicateVertexTolerance && Math.AbsFloat(existingY - screenY) <= duplicateVertexTolerance)
				{
					vertexAlreadyExists = true;
					break;
				}
			}
	
			if (vertexAlreadyExists)
				continue;
	
			screenVertices.Insert(screenX);
			screenVertices.Insert(screenY);
		}
	
		return screenVertices;
	}
	
	protected vector GetShapePosition()
	{
		vector ownerPosition = GetOwner().GetOrigin();
	
		int maxAttempts = 50;
	
		for (int i = 0; i < maxAttempts; i++)
		{
			float randomOffsetX = Math.RandomFloatInclusive(-m_fCurrentRadiusMeters, m_fCurrentRadiusMeters);
			float randomOffsetZ = Math.RandomFloatInclusive(-m_fCurrentRadiusMeters, m_fCurrentRadiusMeters);
	
			vector candidateCenter = ownerPosition + Vector(randomOffsetX, 0, randomOffsetZ);
	
			array<vector> polygonVertices = GetShapeWorld(candidateCenter, m_fCurrentRadiusMeters, m_iPolygonSides);
	
			if (Math2D.IsPointInPolygonXZ(polygonVertices, ownerPosition))
				return candidateCenter;
		}
	
		return ownerPosition;
	}

	protected array<vector> GetShapeWorld(vector centerPosition, float radiusMeters, int sides)
	{
		array<vector> vertices = {};
	
		if (sides < 3)
			return vertices;
	
		float angleStepDegrees = 360.0 / sides;
	
		for (int i = 0; i < sides; i++)
		{
			float angleDegrees = (angleStepDegrees * i) + m_fRotationOffsetDegrees;
			float angleRadians = angleDegrees * Math.DEG2RAD;
	
			float offsetX = Math.Cos(angleRadians) * radiusMeters;
			float offsetZ = Math.Sin(angleRadians) * radiusMeters;
	
			vector worldPoint = centerPosition + Vector(offsetX, 0, offsetZ);
			vertices.Insert(worldPoint);
		}
	
		return vertices;
	}
	
	protected void UpdateRadius()
	{
		if (m_fRadiusChangePerUpdate == 0)
			return;
	
		if (m_fMaxRadiusChangeMeters == 0)
		{
			m_fCurrentRadiusMeters += m_fRadiusChangePerUpdate;
			m_fCurrentRadiusMeters = Math.Max(1, m_fCurrentRadiusMeters);
			return;
		}
	
		float targetRadiusMeters = m_fBaseRadiusMeters;
	
		if (m_fRadiusChangePerUpdate > 0)
			targetRadiusMeters = m_fBaseRadiusMeters + m_fMaxRadiusChangeMeters;
		else if (m_fRadiusChangePerUpdate < 0)
			targetRadiusMeters = m_fBaseRadiusMeters - m_fMaxRadiusChangeMeters;
	
		targetRadiusMeters = Math.Max(1, targetRadiusMeters);
	
		if (m_fCurrentRadiusMeters == targetRadiusMeters)
			return;
	
		m_fCurrentRadiusMeters += m_fRadiusChangePerUpdate;
	
		m_fCurrentRadiusMeters = Math.Clamp(
			m_fCurrentRadiusMeters,
			Math.Min(m_fBaseRadiusMeters, targetRadiusMeters),
			Math.Max(m_fBaseRadiusMeters, targetRadiusMeters)
		);
		
		PrintFormat("GC | UpdateRadius: %1", m_fCurrentRadiusMeters);
	}
	
	protected void UpdateText(array<vector> worldVertices)
	{
		if (m_sMarkerText.IsEmpty() || worldVertices.IsEmpty())
			return;
	
		vector firstPoint = worldVertices[0];
	
		float minX = firstPoint[0];
		float maxX = firstPoint[0];
		float minZ = firstPoint[2];
		float maxZ = firstPoint[2];
	
		for (int i = 1; i < worldVertices.Count(); i++)
		{
			vector point = worldVertices[i];
	
			minX = Math.Min(minX, point[0]);
			maxX = Math.Max(maxX, point[0]);
			minZ = Math.Min(minZ, point[2]);
			maxZ = Math.Max(maxZ, point[2]);
		}
	
		float centerZ = (minZ + maxZ) * 0.5;
		float textPaddingMeters = 10;
	
		vector textWorldPosition = Vector(maxX + textPaddingMeters, firstPoint[1], centerZ);
	
		float screenX;
		float screenY;
	
		m_MapEntity.WorldToScreen(textWorldPosition[0], textWorldPosition[2], screenX, screenY, true);
	
		WorkspaceWidget workspace = GetGame().GetWorkspace();
		FrameSlot.SetPos(m_wTextFrame, workspace.DPIUnscale(screenX), workspace.DPIUnscale(screenY));
	}
	
	protected void Start()
	{
		m_fCurrentRadiusMeters = m_fBaseRadiusMeters;
		m_vMarkerCenter = GetShapePosition();
	
		Rpc(RPC_UpdateMarker, m_vMarkerCenter, m_fCurrentRadiusMeters);
	
		if (RplSession.Mode() != RplMode.Dedicated)
			DrawShape();
	
		if (m_fUpdateIntervalSeconds > 0)
			GetGame().GetCallqueue().CallLater(Update, m_fUpdateIntervalSeconds * 1000);
	}
	
	protected void Update()
	{
		SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		if (gameMode && gameMode.GetState() == SCR_EGameModeState.GAME)
		{
			UpdateRadius();
			m_vMarkerCenter = GetShapePosition();
			
			Rpc(RPC_UpdateMarker, m_vMarkerCenter, m_fCurrentRadiusMeters);
	
			if (RplSession.Mode() != RplMode.Dedicated)
				DrawShape();
		}
	
		if (m_fUpdateIntervalSeconds > 0)
			GetGame().GetCallqueue().CallLater(Update, m_fUpdateIntervalSeconds * 1000);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void RPC_UpdateMarker(vector markerCenter, float radiusMeters)
	{
		m_vMarkerCenter = markerCenter;
		m_fCurrentRadiusMeters = radiusMeters;
		
		if (m_MapEntity && m_MapEntity.IsOpen())
			Setup();
	}
	
	override protected void OnDelete(IEntity owner)
	{
		if (m_wCanvasWidget)
		{
			m_wCanvasWidget.RemoveFromHierarchy();
			m_wCanvasWidget = null;
		}
	
		if (m_wTextFrame)
		{
			m_wTextFrame.RemoveFromHierarchy();
			m_wTextFrame = null;
		}
	}
	
	override bool RplSave(ScriptBitWriter writer)
	{
		writer.WriteVector(m_vMarkerCenter);
		writer.WriteFloat(m_fCurrentRadiusMeters);
		
		return true;
	}
	
	override bool RplLoad(ScriptBitReader reader)
	{
		reader.ReadVector(m_vMarkerCenter);
		reader.ReadFloat(m_fCurrentRadiusMeters);
		
		return true;
	}
}