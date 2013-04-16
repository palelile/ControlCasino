CREATE TABLE "temporal" (
	"anio_trabajo" TEXT,
	"RBD" TEXT,
	"codigo_tipo_ensenanza" TEXT,
	"codigo_grado" TEXT,
	"glosa_grado" TEXT,
	"letra_curso" TEXT,
	"run_alumno" TEXT,
	"dgv_alumno" TEXT,
	"sal_genero" TEXT,
	"nombre_alumno" TEXT,
	"ape_paterno_alumno" TEXT,
	"ape_materno_alumno" TEXT,
	"direccion" TEXT,
	"codigo_region" TEXT,
	"nombre_comuna" TEXT,
	"codigo_comuna" TEXT,
	"telefono" TEXT,
	"celular" TEXT,
	"email" TEXT,
	"fecha_nacimiento" TEXT,
	"cod_etnia" TEXT,
	"fecha_incorporacion" TEXT,
	"fecha_retiro" TEXT,
	"promedio_final" TEXT,
	"porcentaje_asistencia" TEXT
);
CREATE TABLE "alumnos" (
    "id" INTEGER PRIMARY KEY AUTOINCREMENT,
    "rut" INTEGER,
    "nombre" TEXT,
    "apellidoP" TEXT,
    "apellidoM" TEXT,
    "curso" TEXT
);
CREATE UNIQUE INDEX "rut_id" on alumnos (rut ASC);
CREATE TABLE "autorizados" (
    "id" INTEGER PRIMARY KEY AUTOINCREMENT,
    "rut" INTEGER,
    FOREIGN KEY("rut") REFERENCES alumnos(rut)
);
CREATE TABLE "asistenciadesayuno" (
    "id" INTEGER PRIMARY KEY AUTOINCREMENT,
    "rut" INTEGER,
    "fecha" TEXT,
    FOREIGN KEY("rut") REFERENCES alumnos(rut)
);
CREATE TABLE "asistenciaalmuerzo" (
    "id" INTEGER PRIMARY KEY AUTOINCREMENT,
    "rut" INTEGER,
    "fecha" TEXT,
    FOREIGN KEY("rut") REFERENCES alumnos(rut)
);
CREATE TABLE "indices" (
    "id" INTEGER PRIMARY KEY AUTOINCREMENT,
    "tabla" TEXT,
    "ultimoregistrolocal" INTEGER,
    "ultimoregistroremoto" INTEGER
);
