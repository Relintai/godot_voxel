#include "voxel_library.h"

VoxelLibrary::VoxelLibrary() :
		Resource(),
		_atlas_size(1) {

	_voxel_editor_count = 0;
	_voxel_editor_page = 0;
}

VoxelLibrary::~VoxelLibrary() {
	// Handled with a WeakRef
	//	for (unsigned int i = 0; i < MAX_VOXEL_TYPES; ++i) {
	//		if (_voxel_types[i].is_valid()) {
	//			_voxel_types[i]->set_library(NULL);
	//		}
	//	}
}

int VoxelLibrary::get_voxel_count() const {
	int count = 0;
	for (int i = 0; i < MAX_VOXEL_TYPES; ++i) {
		if (_voxel_types[i].is_valid())
			++count;
	}
	return count;
}

void VoxelLibrary::load_default() {
	create_voxel(0, "air")->set_transparent(true);
	create_voxel(1, "solid")
			->set_transparent(false)
			->set_geometry_type(Voxel::GEOMETRY_CUBE);
}

int VoxelLibrary::get_voxel_editor_count() {
	return _voxel_editor_count;
}

void VoxelLibrary::set_voxel_editor_count(int value) {
	_voxel_editor_count = value;
}

int VoxelLibrary::get_voxel_editor_page() {
	return _voxel_editor_page;
}

void VoxelLibrary::set_voxel_editor_page(int value) {
	if (value < 0 || value > (int)(_voxel_editor_count / ITEMS_PER_PAGE)) {
		return;
	}

	_voxel_editor_page = value;
}


void VoxelLibrary::set_atlas_size(int s) {
	ERR_FAIL_COND(s <= 0);
	_atlas_size = s;
}

Ref<Voxel> VoxelLibrary::create_voxel(int id, String name) {
	ERR_FAIL_COND_V(id < 0 || id >= MAX_VOXEL_TYPES, Ref<Voxel>());

	Ref<Voxel> voxel(memnew(Voxel));

	voxel->set_library(Ref<VoxelLibrary>(this));
	voxel->set_id(id);
	voxel->set_voxel_name(name);
	_voxel_types[id] = voxel;

	return voxel;
}

void VoxelLibrary::add_voxel(Ref<Voxel> voxel) {
	int index = get_voxel_count();

	ERR_FAIL_COND(index >= MAX_VOXEL_TYPES);

	voxel->set_id(index);
}

void VoxelLibrary::set_voxel(int id, Ref<Voxel> voxel) {
	ERR_FAIL_COND(id < 0 || id >= MAX_VOXEL_TYPES);

	voxel->set_id(id);

	_voxel_types[id] = voxel;
}

Ref<Voxel> VoxelLibrary::get_voxel(int id) {
	ERR_FAIL_COND_V(id < 0 || id >= MAX_VOXEL_TYPES, Ref<Voxel>());

	return _voxel_types[id];
}

void VoxelLibrary::remove_voxel(int id) {
	ERR_FAIL_COND(id < 0 || id >= MAX_VOXEL_TYPES);

	_voxel_types[id] = Ref<Voxel>(NULL);
}

void VoxelLibrary::_validate_property(PropertyInfo &property) const {
	String prop = property.name;
	if (prop.begins_with("Voxel_")) {
		int frame = prop.get_slicec('/', 0).get_slicec('_', 1).to_int();
		if (frame >= _voxel_editor_count || frame < ITEMS_PER_PAGE * _voxel_editor_page || frame > ITEMS_PER_PAGE * (_voxel_editor_page + 1)) {
			property.usage = 0;
		}
	}
}

void VoxelLibrary::_bind_methods() {

	ClassDB::bind_method(D_METHOD("create_voxel", "id", "name"), &VoxelLibrary::create_voxel);

	ClassDB::bind_method(D_METHOD("set_atlas_size", "square_size"), &VoxelLibrary::set_atlas_size);
	ClassDB::bind_method(D_METHOD("get_atlas_size"), &VoxelLibrary::get_atlas_size);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "atlas_size"), "set_atlas_size", "get_atlas_size");


	ClassDB::bind_method(D_METHOD("get_voxel_editor_count"), &VoxelLibrary::get_voxel_editor_count);
	ClassDB::bind_method(D_METHOD("set_voxel_editor_count", "value"), &VoxelLibrary::set_voxel_editor_count);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "voxel_editor_count", PROPERTY_HINT_RANGE, "0," + itos(MAX_VOXEL_TYPES), PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_UPDATE_ALL_IF_MODIFIED), "set_voxel_editor_count", "get_voxel_editor_count");

	ClassDB::bind_method(D_METHOD("get_voxel_editor_page"), &VoxelLibrary::get_voxel_editor_page);
	ClassDB::bind_method(D_METHOD("set_voxel_editor_page", "value"), &VoxelLibrary::set_voxel_editor_page);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "voxel_editor_page", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_UPDATE_ALL_IF_MODIFIED), "set_voxel_editor_page", "get_voxel_editor_page");

	ClassDB::bind_method(D_METHOD("has_voxel", "id"), &VoxelLibrary::has_voxel);
	ClassDB::bind_method(D_METHOD("add_voxel", "voxel"), &VoxelLibrary::add_voxel);
	ClassDB::bind_method(D_METHOD("remove_voxel", "id"), &VoxelLibrary::set_voxel);
	ClassDB::bind_method(D_METHOD("get_voxel_count"), &VoxelLibrary::get_voxel_count);

	ClassDB::bind_method(D_METHOD("get_voxel", "id"), &VoxelLibrary::get_voxel);
	ClassDB::bind_method(D_METHOD("set_voxel", "id", "voxel"), &VoxelLibrary::set_voxel);

	ADD_GROUP("Voxel", "Voxel");
	for (int i = 0; i < MAX_VOXEL_TYPES; ++i) {
		ADD_PROPERTYI(PropertyInfo(Variant::OBJECT, "Voxel_" + itos(i), PROPERTY_HINT_RESOURCE_TYPE, "Voxel", PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_INTERNAL), "set_voxel", "get_voxel", i);
	}

	BIND_CONSTANT(MAX_VOXEL_TYPES);
}
