#ifndef VOXEL_MESH_UPDATER_H
#define VOXEL_MESH_UPDATER_H

#include <core/os/semaphore.h>
#include <core/os/thread.h>
#include <core/vector.h>

#include "../meshers/blocky/voxel_mesher_blocky.h"
#include "../meshers/dmc/voxel_mesher_dmc.h"
#include "../voxel_buffer.h"

class VoxelMeshUpdater {
public:
	struct InputBlock {
		Ref<VoxelBuffer> voxels;
		Vector3i position;
	};

	struct Input {
		Vector<InputBlock> blocks;
		Vector3i priority_position;

		bool is_empty() const {
			return blocks.empty();
		}
	};

	struct OutputBlock {
		VoxelMesher::Output blocky_surfaces;
		VoxelMesher::Output smooth_surfaces;
		Vector3i position;
	};

	struct Stats {
		bool first;
		uint64_t min_time;
		uint64_t max_time;
		uint32_t remaining_blocks;

		Stats() :
				first(true),
				min_time(0),
				max_time(0),
				remaining_blocks(0) {}
	};

	struct Output {
		Vector<OutputBlock> blocks;
		Stats stats;
	};

	struct MeshingParams {
		bool baked_ao;
		float baked_ao_darkness;
		bool smooth_surface;

		MeshingParams() :
				baked_ao(true),
				baked_ao_darkness(0.75),
				smooth_surface(false) {}
	};

	VoxelMeshUpdater(Ref<VoxelLibrary> library, MeshingParams params);
	~VoxelMeshUpdater();

	void push(const Input &input);
	void pop(Output &output);

	int get_required_padding() const;

private:
	static void _thread_func(void *p_self);
	void thread_func();

	void thread_sync(int queue_index, Stats stats);

	void process_block(const InputBlock &block, OutputBlock &output);

private:
	Input _shared_input;
	Mutex *_input_mutex;
	HashMap<Vector3i, int, Vector3iHasher> _block_indexes;
	bool _needs_sort;

	Output _shared_output;
	Mutex *_output_mutex;

	Ref<VoxelMesherBlocky> _blocky_mesher;
	Ref<VoxelMesherDMC> _dmc_mesher;

	Input _input;
	Output _output;
	Semaphore *_semaphore;
	Thread *_thread;
	bool _thread_exit;
};

#endif // VOXEL_MESH_UPDATER_H
