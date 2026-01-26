class GC_LOSCell
{
	int m_iGX;
	int m_iGY;

	vector m_vCenter;
	vector m_vMin;
	vector m_vMax;

	bool m_bHasLOS;
	bool m_bHasResult = false;

	int m_iLastSeenFrame;
}

class GC_LOSSystem : GameSystem
{
	SCR_MapEntity m_MapEntity;
	
	protected float m_fGridSize = 5;
	protected int m_iRadiusCells = 25;
	protected int m_iKeepRadiusCells = 100;

	protected int m_iFrame;

	protected ref map<string, ref GC_LOSCell> m_mCells = new map<string, ref GC_LOSCell>;
	protected ref array<ref GC_LOSCell> m_aTraceQueue = {};
	
	protected vector m_vStartPosition;
	
	static GC_LOSSystem GetInstance()
	{
		World world = GetGame().GetWorld();
		if (!world)
			return null;
		return GC_LOSSystem.Cast(world.FindSystem(GC_LOSSystem));
	}
	
	override static void InitInfo(WorldSystemInfo outInfo)
	{
		super.InitInfo(outInfo);
		outInfo
			.SetAbstract(false)
			.SetUnique(true)
			.SetLocation(WorldSystemLocation.Client)
	}
	
	protected void Init()
	{
		m_MapEntity = SCR_MapEntity.GetMapInstance();
		
		if (!m_MapEntity)
			return;
		
		m_MapEntity.GetOnMapOpen().Insert(OnMapOpen);
		m_MapEntity.GetOnMapClose().Insert(OnMapClose);
	}
	
	protected void OnMapOpen(MapConfiguration mapConfig)
	{
		SetState(true);
	}	
	
	protected void OnMapClose(MapConfiguration mapConfig)
	{
		SetState(false);
	}

	void UpdateAroundCursor(float cursorX, float cursorZ)
	{
		m_iFrame++;

		int cGX = WorldToGrid(cursorX);
		int cGZ = WorldToGrid(cursorZ);

		array<GC_LOSCell> cells = MarkWantedCells(cGX, cGZ, m_iRadiusCells);
		PruneOldCells(cGX, cGZ, m_iKeepRadiusCells);

		ProcessTraceQueue(1000); // traces per update (tune)
		
		GC_LOSCheckerUI losMenu = GC_LOSCheckerUI.Cast(m_MapEntity.GetMapUIComponent(GC_LOSCheckerUI));
		losMenu.RegisterCells(cells);
		//PrintFormat("GC | losMenu: %1", losMenu);
	}
	
	protected array<GC_LOSCell> MarkWantedCells(int cGX, int cGZ, int radiusCells)
	{
		array<GC_LOSCell> cells = {};
		
		int rSq = radiusCells * radiusCells;

		for (int ix = -radiusCells; ix <= radiusCells; ix++)
		{
			for (int iz = -radiusCells; iz <= radiusCells; iz++)
			{
				if (ix * ix + iz * iz > rSq)
					continue;

				int gx = cGX + ix;
				int gy = cGZ + iz;

				string key = MakeKey(gx, gy);
				
				GC_LOSCell cell = m_mCells.Get(key);
				if (!cell)
				{
					cell = new GC_LOSCell();
					cell.m_iGX = gx;
					cell.m_iGY = gy;

					BuildCellBounds(cell);
					
					m_mCells.Set(key, cell);
					//PrintFormat("GC | Newcell: %1, %2 - Pos: %3", cell.m_iGX, cell.m_iGY, cell.m_vCenter);
					m_aTraceQueue.Insert(cell);
				}

				cells.Insert(cell);
				cell.m_iLastSeenFrame = m_iFrame;
			}
		}
		
		return cells;
	}
	
	protected void PruneOldCells(int cGX, int cGZ, int keepRadiusCells)
	{
		int keepSq = keepRadiusCells * keepRadiusCells;

		ref array<string> toRemove = {};

		foreach (string key, GC_LOSCell cell : m_mCells)
		{
			// If not recently seen, drop it
			if (cell.m_iLastSeenFrame != m_iFrame)
			{
				int dx = cell.m_iGX - cGX;
				int dz = cell.m_iGY - cGZ;

				if (dx * dx + dz * dz > keepSq)
					toRemove.Insert(key);
			}
		}

		foreach (string k : toRemove)
			m_mCells.Remove(k);
	}
	
	protected void ProcessTraceQueue(int maxTraces)
	{
		int count = Math.Min(maxTraces, m_aTraceQueue.Count());
			
		for (int i = 0; i < count; i++)
		{
			GC_LOSCell cell = m_aTraceQueue[0];
			m_aTraceQueue.RemoveOrdered(0);

			if (!cell)
				continue;

			DoLOSTrace(cell);
		}
	}

	protected void DoLOSTrace(GC_LOSCell cell)
	{
		// Replace this with your real LOS trace:
		// start = your observer position (player camera / unit head)
		// end = vector m_vCenter; + some height
		vector end = cell.m_vCenter;
		
		bool hasLOS = HasTerrainLOS(cell.m_vCenter);

		cell.m_bHasLOS = hasLOS;
		cell.m_bHasResult = true;
	}
	
	protected bool HasTerrainLOS(vector toPos)
	{
		BaseWorld world = GetGame().GetWorld();
		if (!world)
			return false;
	
		TraceParam trace = new TraceParam();
		trace.Start = m_vStartPosition;
		trace.End = toPos;
		trace.TargetLayers = EPhysicsLayerDefs.Terrain;
		trace.Flags = TraceFlags.WORLD | TraceFlags.ANY_CONTACT;
	
		float frac = world.TraceMove(trace);
	
		if (frac >= 0.9999)
			return true;
	
		// If we "hit" basically at the end, treat as clear (end is on/near terrain)
		vector dir = trace.End - trace.Start;
		vector hitPos = trace.Start + dir * frac;
	
		if (vector.DistanceSq(hitPos, trace.End) <= 1)
			return true;
	
		return false;
	}
	
	protected void BuildCellBounds(GC_LOSCell cell)
	{
		float minWorldX = (cell.m_iGX * m_fGridSize);
		float minWorldZ = (cell.m_iGY * m_fGridSize);
	
		float maxWorldX = minWorldX + m_fGridSize;
		float maxWorldZ = minWorldZ + m_fGridSize;
	
		cell.m_vMin = Vector(minWorldX, 0, minWorldZ);
		cell.m_vMax = Vector(maxWorldX, 0, maxWorldZ);
	
		cell.m_vCenter = Vector((minWorldX + maxWorldX) * 0.5, 0, (minWorldZ + maxWorldZ) * 0.5);
	
		// Cache ground height at the center
		cell.m_vCenter[1] = GetGame().GetWorld().GetSurfaceY(cell.m_vCenter[0], cell.m_vCenter[2]) + 0.1;
	}
	
	protected int WorldToGrid(float world)
	{
		return Math.Floor(world / m_fGridSize);
	}

	protected float GridToWorld(int g)
	{
		return g * m_fGridSize;
	}

	protected string MakeKey(int gx, int gy)
	{
		return string.Format("%1:%2", gx, gy);
	}
	
	void SetStart(vector startPostion, float offsetY = 1.5)
	{
		startPostion[1] = startPostion[1]  + offsetY;
		
		m_vStartPosition = startPostion;
		
		m_mCells.Clear();
		m_aTraceQueue.Clear();
		m_MapEntity = SCR_MapEntity.GetMapInstance();
		Enable(true);
	}
	
	void SetState(bool enabled)
	{
		Enable(enabled);
		
		m_mCells.Clear();
	}
}