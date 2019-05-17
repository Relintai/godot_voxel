#include "voxel_library.h"

VoxelLibrary::VoxelLibrary() :
		Resource(),
		_atlas_size(1) {

	_voxel_editor_count = 0;
	_voxel_editor_page = 0;

	_atlas_rows = 8;
	_atlas_columns = 8;
	_is_textured = true;

	_uvs = memnew(Vector<Vector<Vector2> >());

	//rebuild_uvs();
}

VoxelLibrary::~VoxelLibrary() {
	// Handled with a WeakRef
	//	for (unsigned int i = 0; i < MAX_VOXEL_TYPES; ++i) {
	//		if (_voxel_types[i].is_valid()) {
	//			_voxel_types[i]->set_library(NULL);
	//		}
	//	}

	_uvs->clear();

	memdelete(_uvs);
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

void VoxelLibrary::rebuild_uvs() {
	float material_width = (float)1 / (float)(get_atlas_columns());
	float material_height = (float)1 / (float)(get_atlas_rows());

	_uvs->clear();

	for (float num2 = (float)0; num2 < (float)1; num2 += material_height) {
		for (float num3 = (float)0; num3 < (float)1; num3 += material_width) {
			_uvs->push_back(get_uvs_test(num3, num2, material_width, material_height));
		}
	}
}

Vector<Vector2> VoxelLibrary::get_material_uv(int ID) {
	if (_is_textured) {
		return (*_uvs)[ID];
	}

	return get_uvs_test((float)0, (float)0, (float)1, (float)1);
}

Vector<Vector2> VoxelLibrary::get_uvs_test(float x, float y, float w, float h) {

	Vector<Vector2> v;

	v.push_back(Vector2(x + w, y));
	v.push_back(Vector2(x + w, y + h));
	v.push_back(Vector2(x, y + h));
	v.push_back(Vector2(x, y));

	return v;
}

void VoxelLibrary::set_atlas_columns(int s) {
	ERR_FAIL_COND(s <= 0);
	_atlas_columns = s;
}

void VoxelLibrary::set_atlas_rows(int s) {
	ERR_FAIL_COND(s <= 0);
	_atlas_rows = s;
}

bool VoxelLibrary::get_is_textured() const {
	return _is_textured;
}

void VoxelLibrary::set_is_textured(bool value) {
	_is_textured = value;
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

	ClassDB::bind_method(D_METHOD("get_atlas_columns"), &VoxelLibrary::get_atlas_columns);
	ClassDB::bind_method(D_METHOD("set_atlas_columns", "value"), &VoxelLibrary::set_atlas_columns);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "atlas_columns"), "set_atlas_columns", "get_atlas_columns");

	ClassDB::bind_method(D_METHOD("get_atlas_rows"), &VoxelLibrary::get_atlas_rows);
	ClassDB::bind_method(D_METHOD("set_atlas_rows", "value"), &VoxelLibrary::set_atlas_rows);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "atlas_rows"), "set_atlas_rows", "get_atlas_rows");

	ClassDB::bind_method(D_METHOD("get_is_textured"), &VoxelLibrary::get_is_textured);
	ClassDB::bind_method(D_METHOD("set_is_textured", "value"), &VoxelLibrary::set_is_textured);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_textured"), "set_is_textured", "get_is_textured");

	ClassDB::bind_method(D_METHOD("get_material"), &VoxelLibrary::get_material);
	ClassDB::bind_method(D_METHOD("set_material", "value"), &VoxelLibrary::set_material);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "material", PROPERTY_HINT_RESOURCE_TYPE, "Material"), "set_material", "get_material");

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
