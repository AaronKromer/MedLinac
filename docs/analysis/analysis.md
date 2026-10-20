Overview

This project uses AIDA (via the included helper code) to produce an XML file (MedLinacDiane.xml) that contains a set of histograms for post-run analysis. The two main analysis types produced by the example are:

- PDD (Percentage Depth Dose): dose as a function of depth along the beam axis (z).
- Lateral dose profiles (transverse profiles): dose across x (or y) at selected depths.

Because the example supports multi-threaded runs, histograms are not filled directly inside `ProcessHits()` (which executes very frequently and can run concurrently on many threads). Instead, the code buffers raw hit information per event and collects these hits into a run-level collection; at the end of the run the collected hits are processed to fill the AIDA histograms.

Implementation pipeline (where to find it in the sources)

1. Hit creation (sensitive detector)
	- `MedLinacPhantomSD::ProcessHits()` computes voxel indices and the physical position for the step and constructs a `MedLinacHit` struct containing `(x,y,z,energyDep)`.
	- Instead of writing directly to histograms, the SD forwards each hit to the current `EventAction` instance: `MedLinacEventAction::AddHit()`.

2. Per-event buffering (event action)
	- `MedLinacEventAction` keeps a per-event `hitsCollection` (cleared in `BeginOfEventAction`).
	- In `EndOfEventAction()` the event-local hits are forwarded to the current worker `MedLinacRun` (via `MedLinacRun::AddHits()`), which accumulates a run-level vector that will be merged on the master via `MedLinacRun::Merge()`.

3. Run-level processing (run action)
	- At `BeginOfRunAction()` the `MedLinacAnalysisManager::book()` method is called to create histograms.
	- At `EndOfRunAction()` the `MedLinacRunAction` iterates over `runHitsCollection` and calls `AnalyzeHit()` for each hit; `AnalyzeHit()` maps hits to the target histograms via calls such as `analysis->FillHistogram1WithEnergy(...)`.
	- After all hits are analyzed, `MedLinacAnalysisManager::finish()` writes the AIDA XML output.

Why this design?

- Filling AIDA histograms from inside `ProcessHits()` in a multi-threaded run can cause concurrent access problems: AIDA objects (and older analysis backends) are frequently not thread-safe and cannot be modified safely from many threads in parallel.
- Buffering hits and performing analysis in a single place avoids fine-grained locking during the hot path (`ProcessHits()`), at the cost of more memory for the buffered hits.

Thread-safety and performance notes

- Current behaviour: event hits are appended into the worker `MedLinacRun` instance. The worker runs are merged on the master via `MedLinacRun::Merge()`, so no global locking is required for pushing event hits.
- Options to make this robust:
  - Simple (safe): add a `std::mutex` inside `MedLinacAnalysisManager` and lock it inside `AddHit()` (use `std::lock_guard<std::mutex>`). This ensures correctness with minimal code changes but may introduce contention if there are many hits.
  - Better for performance: keep a thread-local buffer of hits (e.g. `thread_local std::vector<MedLinacHit>`) and merge the per-thread buffers at the end of the run (or use a custom `G4Run` with `Merge()` for automatic merging). This reduces contention and is preferred for high-rate simulations.
  - Use a thread-safe analysis backend or a thread-local instance of the analysis manager and merge histograms at the end of the run.

Where histograms are created and filled

- Histograms are created in `MedLinacAnalysisManager::book()` (called from `MedLinacRunAction::BeginOfRunAction`).
- Hit → histogram mapping is implemented in `MedLinacRunAction::AnalyzeHit()` (called from `EndOfRunAction`), which performs checks on hit coordinates (x, y, z) and calls `FillHistogram*WithEnergy()` on the analysis manager.


| Histogram code | Histogram ID | Histogram name            | Type | Scanning axis | Depth / z condition | Selection window | Note |
|----------------|--------------|---------------------------|------|---------------|---------------------|------------------|------|
| 14200          | hBase        | base                      | Generic | x | No z selection | No x,y,z selection | Base histogram |
| 14010          | h1           | PDD5                      | PDD  | z | Isocenter / central slice | x,y ∈ [-2.5, 2.5] mm | Percent depth dose |
| 14020          | h2           | Flatness build-up5        | LDP  | x | z ∈ [132.5, 137.5] mm, center ≈ 135 mm | y ∈ [-2.5, 2.5] mm | Lateral dose profile at build-up, about 15 mm depth |
| 14030          | h3           | Flatness 50mm 5           | LDP  | x | z ∈ [97.5, 102.5] mm, center ≈ 100 mm | y ∈ [-2.5, 2.5] mm | Lateral dose profile at 50 mm depth |
| 14040          | h4           | Flatness 100mm 5          | LDP  | x | z ∈ [47.5, 52.5] mm, center ≈ 50 mm | y ∈ [-2.5, 2.5] mm | Lateral dose profile at 100 mm depth |
| 14050          | h5           | Flatness 200mm 5          | LDP  | x | z ∈ [-52.5, -47.5] mm, center ≈ -50 mm | y ∈ [-2.5, 2.5] mm | Lateral dose profile at 200 mm depth |
| 14060          | h6           | PDD 2                     | PDD  | z | Isocenter / central slice | x,y ∈ [-2.5, 2.5] mm | PDD replica |
| 14070          | h7           | Flatness build-up 2       | LDP  | x | z ∈ [132.5, 137.5] mm, center ≈ 135 mm | y ∈ [-2.5, 2.5] mm | Replica |
| 14080          | h8           | Flatness 50mm 2           | LDP  | x | z ∈ [97.5, 102.5] mm, center ≈ 100 mm | y ∈ [-2.5, 2.5] mm | Replica |
| 14090          | h9           | Flatness 100mm 2          | LDP  | x | z ∈ [47.5, 52.5] mm, center ≈ 50 mm | y ∈ [-2.5, 2.5] mm | Replica |
| 14100          | h10          | Flatness 200mm 2          | LDP  | x | z ∈ [-52.5, -47.5] mm, center ≈ -50 mm | y ∈ [-2.5, 2.5] mm | Replica |
| 14101          | h11          | PDD 1                     | PDD  | z | Isocenter / central slice | x,y ∈ [-2.5, 2.5] mm | PDD replica |
| 14102          | h12          | Flatness build-up 1       | LDP  | x | z ∈ [132.5, 137.5] mm, center ≈ 135 mm | y ∈ [-2.5, 2.5] mm | Replica |
| 14103          | h13          | Flatness 50mm 1           | LDP  | x | z ∈ [97.5, 102.5] mm, center ≈ 100 mm | y ∈ [-2.5, 2.5] mm | Replica |
| 14104          | h14          | Flatness 100mm 1          | LDP  | x | z ∈ [47.5, 52.5] mm, center ≈ 50 mm | y ∈ [-2.5, 2.5] mm | Replica |
| 14105          | h15          | Flatness 200mm 1          | LDP  | x | z ∈ [-52.5, -47.5] mm, center ≈ -50 mm | y ∈ [-2.5, 2.5] mm | Replica |
References

- AIDA R package: https://github.com/apfeiffer1/aidar/