#ifndef VOXEL_LIBRARY_H
#define VOXEL_LIBRARY_H

#include "voxel.h"
#include <core/resource.h>

#include "scene/resources/material.h"

class VoxelLibrary : public Resource {
	GDCLASS(VoxelLibrary, Resource)

public:
	static const unsigned int MAX_VOXEL_TYPES = 256; // Required limit because voxel types are stored in 8 bits
	static const unsigned int ITEMS_PER_PAGE = 256; //TODO fix saving items that are not on the currently active page

	VoxelLibrary();
	~VoxelLibrary();

	int get_atlas_size() const { return _atlas_size; }
	void set_atlas_size(int s);

	Ref<Voxel> create_voxel(int id, String name);

	void add_voxel(Ref<Voxel> voxel);
	void set_voxel(int id, Ref<Voxel> voxel);
	Ref<Voxel> get_voxel(int id);
	void remove_voxel(int id);

	int get_voxel_count() const;

	void load_default();

	int get_voxel_editor_count();
	void set_voxel_editor_count(int value);

	int get_voxel_editor_page();
	void set_voxel_editor_page(int value);

	_FORCE_INLINE_ bool has_voxel(int id) const { return _voxel_types[id].is_valid(); }
	_FORCE_INLINE_ const Voxel &get_voxel_const(int id) const { return **_voxel_types[id]; }

	//Atlas
	int get_atlas_columns() const { return _atlas_columns; }
	void set_atlas_columns(int s);

	int get_atlas_rows() const { return _atlas_rows; }
	void set_atlas_rows(int s);

	bool get_is_textured() const;
	void set_is_textured(bool s);

	Ref<Material> get_material() const { return _material; }
	void set_material(Ref<Material> mat) { _material = mat; }

	void rebuild_uvs();
	Vector<Vector2> get_material_uv(int ID);
	static Vector<Vector2> get_uvs_test(float x, float y, float w, float h);

protected:
	void _validate_property(PropertyInfo &property) const;
	static void _bind_methods();

private:
	int _voxel_editor_count;
	int _voxel_editor_page;

	Ref<Voxel> _voxel_types[MAX_VOXEL_TYPES];

	//atlas
	int _atlas_columns;
	int _atlas_rows;
	bool _is_textured;

	Vector<Vector<Vector2> > *_uvs;

	Ref<Material> _material;

	int _atlas_size;
};

#endif // VOXEL_LIBRARY_H
