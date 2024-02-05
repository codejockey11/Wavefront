
#include "../../dx12Engine/dx12Engine/standard.h"

#define VERTEX_COUNT 65536
#define LINE_LENGTH 132
#define MATERIAL_COUNT 8
#define MATERIAL_LENGTH 32

CHAR filename[LINE_LENGTH];

CHAR materialFilename[LINE_LENGTH];

CHAR line[LINE_LENGTH];

CHAR path[LINE_LENGTH];

CHAR materialName[MATERIAL_COUNT][MATERIAL_LENGTH];

FILE* file;
FILE* out;

errno_t err;

INT br;

INT inbr;
INT mnbr;

UINT icount[MATERIAL_COUNT];

INT vntcount;

INT vindex;
INT uvindex;
INT nindex;

INT maxIndex;

VOID WriteMaterials(FILE* f, int mc);

VOID WriteVertices(FILE* f, XMFLOAT3 p, int mc);

VOID TextureName(CONST CHAR* path, CHAR* str);

XMFLOAT3 vertex[VERTEX_COUNT];
XMFLOAT3 uv[VERTEX_COUNT];
XMFLOAT3 normal[VERTEX_COUNT];

INT vi[8];
INT uvi[8];
INT ni[8];

class Vertex
{
public:

	XMFLOAT3 p;
	XMFLOAT3 n;
	XMFLOAT2 uv;
};

class Material
{
public:

	CHAR name[MATERIAL_LENGTH];

	XMFLOAT3 ambient;           // Ka
	XMFLOAT3 diffuse;           // Kd
	XMFLOAT3 specular;          // Ks
	XMFLOAT3 emissive;          // Ke

	FLOAT    specularExponent;  // Ns

	FLOAT    opacity;           // d or inverted: Tr = 1 - d

	UINT     illum;

	FLOAT	 opticalDensity;    // Specifies the optical density for the surface.This is also known as index of refraction.

	CHAR map_Ka[MATERIAL_LENGTH];	// the ambient texture map
	CHAR map_Kd[MATERIAL_LENGTH];	// the diffuse texture map(most of the time, it will be the same as the ambient texture map
	CHAR map_Ks[MATERIAL_LENGTH];   // specular color texture map
	CHAR map_Ns[MATERIAL_LENGTH];   // specular highlight component
	CHAR map_d[MATERIAL_LENGTH];    // the alpha texture map
	CHAR map_Kb[MATERIAL_LENGTH];   // some implementations use 'map_bump' instead of 'bump' below

	CHAR refl[MATERIAL_LENGTH];     // spherical reflection map

	Material()
	{
		memset(this, 0x00, sizeof(Material));
	}
};

Vertex vnt[MATERIAL_COUNT][VERTEX_COUNT];

Material mat[MATERIAL_COUNT];

int main(int argc, char** argv)
{
	memset(filename, 0x00, LINE_LENGTH);
	memset(materialFilename, 0x00, LINE_LENGTH);

	memcpy(filename, argv[1], strlen(argv[1]));

	memcpy(materialFilename, argv[2], strlen(argv[2]));



	err = fopen_s(&file, filename, "r");

	if (err)
	{
		printf("fopen_s:%s\n", filename);

		return -1;
	}

	BYTE ft = TRUE;

	inbr = -1;

	if (file)
	{
		while (fscanf_s(file, "%s", line, LINE_LENGTH) != EOF)
		{
			if (strcmp(line, "usemtl") == 0)
			{
				inbr++;

				br = fscanf_s(file, "%s\n", &materialName[inbr], MATERIAL_LENGTH);

				if (ft)
				{
					ft = FALSE;
				}
				else if (inbr > 0)
				{
					icount[inbr - 1] = vntcount;

					vntcount = 0;
				}
			}

			if (strcmp(line, "v") == 0)
			{
				br = fscanf_s(file, "%f %f %f\n", &vertex[vindex].x, &vertex[vindex].y, &vertex[vindex].z);

				vindex++;
			}

			if (strcmp(line, "vt") == 0)
			{
				br = fscanf_s(file, "%f %f\n", &uv[uvindex].x, &uv[uvindex].y);

				uvindex++;
			}

			if (strcmp(line, "vn") == 0)
			{
				br = fscanf_s(file, "%f %f %f\n", &normal[nindex].x, &normal[nindex].y, &normal[nindex].z);

				nindex++;
			}

			if (strcmp(line, "f") == 0)
			{
				for (UINT i = 0; i < 8; i++)
				{
					vi[i] = uvi[i] = ni[i] = 0;
				}

				br = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n",
					&vi[0], &uvi[0], &ni[0],
					&vi[1], &uvi[1], &ni[1],
					&vi[2], &uvi[2], &ni[2],
					&vi[3], &uvi[3], &ni[3],
					&vi[4], &uvi[4], &ni[4],
					&vi[5], &uvi[5], &ni[5],
					&vi[6], &uvi[6], &ni[6],
					&vi[7], &uvi[7], &ni[7]
				);

				for (UINT i = 0; i < 3; i++)
				{
					if ((vi[i] > 0) && (ni[i] > 0) && (uvi[i] > 0))
					{
						vnt[inbr][vntcount].p.x = vertex[vi[i] - 1].x;
						vnt[inbr][vntcount].p.y = vertex[vi[i] - 1].y;
						vnt[inbr][vntcount].p.z = vertex[vi[i] - 1].z;

						vnt[inbr][vntcount].n.x = normal[ni[i] - 1].x;
						vnt[inbr][vntcount].n.y = normal[ni[i] - 1].y;
						vnt[inbr][vntcount].n.z = normal[ni[i] - 1].z;

						vnt[inbr][vntcount].uv.x = uv[uvi[i] - 1].x;
						vnt[inbr][vntcount].uv.y = uv[uvi[i] - 1].y;

						vntcount++;
					}
				}

				maxIndex = 0;

				for (UINT i = 3; i < 8; i++)
				{
					if (vi[i] != 0)
					{
						maxIndex = i - 2;
					}
				}

				for (int i = 0; i < maxIndex; i++)
				{
					vnt[inbr][vntcount].p.x = vertex[vi[i] - 1].x;
					vnt[inbr][vntcount].p.y = vertex[vi[i] - 1].y;
					vnt[inbr][vntcount].p.z = vertex[vi[i] - 1].z;

					vnt[inbr][vntcount].n.x = normal[ni[i] - 1].x;
					vnt[inbr][vntcount].n.y = normal[ni[i] - 1].y;
					vnt[inbr][vntcount].n.z = normal[ni[i] - 1].z;

					vnt[inbr][vntcount].uv.x = uv[uvi[i] - 1].x;
					vnt[inbr][vntcount].uv.y = uv[uvi[i] - 1].y;

					vntcount++;


					vnt[inbr][vntcount].p.x = vertex[vi[i + 2] - 1].x;
					vnt[inbr][vntcount].p.y = vertex[vi[i + 2] - 1].y;
					vnt[inbr][vntcount].p.z = vertex[vi[i + 2] - 1].z;

					vnt[inbr][vntcount].n.x = normal[ni[i + 2] - 1].x;
					vnt[inbr][vntcount].n.y = normal[ni[i + 2] - 1].y;
					vnt[inbr][vntcount].n.z = normal[ni[i + 2] - 1].z;

					vnt[inbr][vntcount].uv.x = uv[uvi[i + 2] - 1].x;
					vnt[inbr][vntcount].uv.y = uv[uvi[i + 2] - 1].y;

					vntcount++;


					vnt[inbr][vntcount].p.x = vertex[vi[i + 3] - 1].x;
					vnt[inbr][vntcount].p.y = vertex[vi[i + 3] - 1].y;
					vnt[inbr][vntcount].p.z = vertex[vi[i + 3] - 1].z;

					vnt[inbr][vntcount].n.x = normal[ni[i + 3] - 1].x;
					vnt[inbr][vntcount].n.y = normal[ni[i + 3] - 1].y;
					vnt[inbr][vntcount].n.z = normal[ni[i + 3] - 1].z;

					vnt[inbr][vntcount].uv.x = uv[uvi[i + 3] - 1].x;
					vnt[inbr][vntcount].uv.y = uv[uvi[i + 3] - 1].y;

					vntcount++;
				}
			}
		}

		if (inbr < MATERIAL_COUNT)
		{
			icount[inbr] = vntcount;
		}

		fclose(file);
	}



	err = fopen_s(&file, materialFilename, "r");

	if (err)
	{
		printf("fopen_s:%s\n", materialFilename);

		return -1;
	}

	mnbr = -1;

	if (file)
	{
		while (fscanf_s(file, "%s", line, LINE_LENGTH) != EOF)
		{
			if (strcmp(line, "newmtl") == 0)
			{
				mnbr++;

				memset(mat[mnbr].name, 0x00, MATERIAL_LENGTH);

				br = fscanf_s(file, "%s", &mat[mnbr].name, MATERIAL_LENGTH);
			}

			if (strcmp(line, "Ka") == 0)
			{
				br = fscanf_s(file, "%f %f %f\n", &mat[mnbr].ambient.x, &mat[mnbr].ambient.y, &mat[mnbr].ambient.z);
			}

			if (strcmp(line, "Kd") == 0)
			{
				br = fscanf_s(file, "%f %f %f\n", &mat[mnbr].diffuse.x, &mat[mnbr].diffuse.y, &mat[mnbr].diffuse.z);
			}

			if (strcmp(line, "Ks") == 0)
			{
				br = fscanf_s(file, "%f %f %f\n", &mat[mnbr].specular.x, &mat[mnbr].specular.y, &mat[mnbr].specular.z);
			}

			if (strcmp(line, "Ke") == 0)
			{
				br = fscanf_s(file, "%f %f %f\n", &mat[mnbr].emissive.x, &mat[mnbr].emissive.y, &mat[mnbr].emissive.z);
			}

			if (strcmp(line, "Ni") == 0)
			{
				br = fscanf_s(file, "%f\n", &mat[mnbr].opticalDensity);
			}

			if (strcmp(line, "d") == 0)
			{
				br = fscanf_s(file, "%f\n", &mat[mnbr].opacity);
			}

			if (strcmp(line, "illum") == 0)
			{
				br = fscanf_s(file, "%i\n", &mat[mnbr].illum);
			}

			if (strcmp(line, "map_Kd") == 0)
			{
				UINT c = 0;

				memset((VOID*)path, 0x00, LINE_LENGTH);

				while (path[c] != '\n')
				{
					br = fscanf_s(file, "%c", &path[c], 1);

					if (path[c] == '\n')
					{
						path[c] = 0x00;

						memset(mat[mnbr].map_Kd, 0x00, MATERIAL_LENGTH);

						TextureName(path, mat[mnbr].map_Kd);

						break;
					}
					else if (path[c] != ' ')
					{
						c++;
					}
				}
			}

			if (strcmp(line, "map_Ks") == 0)
			{
				UINT c = 0;

				memset((VOID*)path, 0x00, LINE_LENGTH);

				while (path[c] != '\n')
				{
					br = fscanf_s(file, "%c", &path[c], 1);

					if (path[c] == '\n')
					{
						path[c] = 0x00;

						memset(mat[mnbr].map_Ks, 0x00, MATERIAL_LENGTH);

						TextureName(path, mat[mnbr].map_Ks);

						break;
					}
					else if (path[c] != ' ')
					{
						c++;
					}
				}
			}

			if (strcmp(line, "map_Ka") == 0)
			{
				UINT c = 0;

				memset((VOID*)path, 0x00, LINE_LENGTH);

				while (path[c] != '\n')
				{
					br = fscanf_s(file, "%c", &path[c], 1);

					if (path[c] == '\n')
					{
						path[c] = 0x00;

						memset(mat[mnbr].map_Ka, 0x00, MATERIAL_LENGTH);

						TextureName(path, mat[mnbr].map_Ka);

						break;
					}
					else if (path[c] != ' ')
					{
						c++;
					}
				}
			}

			if (strcmp(line, "map_Bump") == 0)
			{
				UINT c = 0;

				memset((VOID*)path, 0x00, LINE_LENGTH);

				while (path[c] != '\n')
				{
					br = fscanf_s(file, "%c", &path[c], 1);

					if (path[c] == '\n')
					{
						path[c] = 0x00;

						memset(mat[mnbr].map_Kb, 0x00, MATERIAL_LENGTH);

						TextureName(path, mat[mnbr].map_Kb);

						break;
					}
					else if (path[c] != ' ')
					{
						c++;
					}
				}
			}

			if (strcmp(line, "refl") == 0)
			{
				UINT c = 0;

				memset((VOID*)path, 0x00, LINE_LENGTH);

				while (path[c] != '\n')
				{
					br = fscanf_s(file, "%c", &path[c], 1);

					if (path[c] == '\n')
					{
						path[c] = 0x00;

						memset(mat[mnbr].refl, 0x00, MATERIAL_LENGTH);

						TextureName(path, mat[mnbr].refl);

						break;
					}
					else if (path[c] != ' ')
					{
						c++;
					}
				}
			}
		}

		fclose(file);
	}


	SHORT mcount = mnbr + 1;

	fopen_s(&out, "out.model", "wb");

	fwrite(&mcount, sizeof(SHORT), 1, out);

	for (INT mc = 0; mc < mcount; mc++)
	{
		WriteMaterials(out, mc);

		WriteVertices(out, XMFLOAT3(0.0F, 0.0F, 0.0F), mc);
	}

	if (out)
	{
		fclose(out);
	}

	return 0;
}

VOID WriteMaterials(FILE* f, int mc)
{
	if (f == NULL)
	{
		return;
	}

	SHORT len = (SHORT)strlen(mat[mc].name);
	fwrite(&len, sizeof(SHORT), 1, f);
	fwrite(mat[mc].name, sizeof(CHAR), len, f);

	fwrite(&mat[mc].ambient.x, sizeof(FLOAT), 1, f);
	fwrite(&mat[mc].ambient.y, sizeof(FLOAT), 1, f);
	fwrite(&mat[mc].ambient.z, sizeof(FLOAT), 1, f);

	fwrite(&mat[mc].diffuse.x, sizeof(FLOAT), 1, f);
	fwrite(&mat[mc].diffuse.y, sizeof(FLOAT), 1, f);
	fwrite(&mat[mc].diffuse.z, sizeof(FLOAT), 1, f);

	fwrite(&mat[mc].specular.x, sizeof(FLOAT), 1, f);
	fwrite(&mat[mc].specular.y, sizeof(FLOAT), 1, f);
	fwrite(&mat[mc].specular.z, sizeof(FLOAT), 1, f);

	fwrite(&mat[mc].emissive.x, sizeof(FLOAT), 1, f);
	fwrite(&mat[mc].emissive.y, sizeof(FLOAT), 1, f);
	fwrite(&mat[mc].emissive.z, sizeof(FLOAT), 1, f);

	fwrite(&mat[mc].specularExponent, sizeof(FLOAT), 1, f);

	fwrite(&mat[mc].opacity, sizeof(FLOAT), 1, f);

	fwrite(&mat[mc].illum, sizeof(UINT), 1, f);

	fwrite(&mat[mc].opticalDensity, sizeof(FLOAT), 1, f);

	len = (SHORT)strlen(mat[mc].map_Ka);
	fwrite(&len, sizeof(SHORT), 1, f);
	fwrite(&mat[mc].map_Ka, sizeof(CHAR), len, f);

	len = (SHORT)strlen(mat[mc].map_Kd);
	fwrite(&len, sizeof(SHORT), 1, f);
	fwrite(&mat[mc].map_Kd, sizeof(CHAR), len, f);

	len = (SHORT)strlen(mat[mc].map_Ks);
	fwrite(&len, sizeof(SHORT), 1, f);
	fwrite(&mat[mc].map_Ks, sizeof(CHAR), len, f);

	len = (SHORT)strlen(mat[mc].map_Ns);
	fwrite(&len, sizeof(SHORT), 1, f);
	fwrite(&mat[mc].map_Ns, sizeof(CHAR), len, f);

	len = (SHORT)strlen(mat[mc].map_d);
	fwrite(&len, sizeof(SHORT), 1, f);
	fwrite(&mat[mc].map_d, sizeof(CHAR), len, f);

	len = (SHORT)strlen(mat[mc].map_Kb);
	fwrite(&len, sizeof(SHORT), 1, f);
	fwrite(&mat[mc].map_Kb, sizeof(CHAR), len, f);

	len = (SHORT)strlen(mat[mc].refl);
	fwrite(&len, sizeof(SHORT), 1, f);
	fwrite(&mat[mc].refl, sizeof(CHAR), len, f);
}

VOID WriteVertices(FILE* f, XMFLOAT3 p, int mc)
{
	if (f == NULL)
	{
		return;
	}

	XMFLOAT3 pos;
	XMFLOAT3 nrm;
	XMFLOAT3 uv;

	fwrite(&icount[mc], sizeof(UINT), 1, f);

	for (UINT vc = 0; vc < icount[mc]; vc++)
	{
		pos.x = vnt[mc][vc].p.x + p.x;
		pos.y = vnt[mc][vc].p.y + p.y;
		pos.z = vnt[mc][vc].p.z + p.z;

		nrm.x = vnt[mc][vc].n.x;
		nrm.y = vnt[mc][vc].n.y;
		nrm.z = vnt[mc][vc].n.z;

		uv.x = vnt[mc][vc].uv.x;
		uv.y = vnt[mc][vc].uv.y;

		fwrite(&pos.x, sizeof(FLOAT), 1, f);
		fwrite(&pos.y, sizeof(FLOAT), 1, f);
		fwrite(&pos.z, sizeof(FLOAT), 1, f);

		fwrite(&nrm.x, sizeof(FLOAT), 1, f);
		fwrite(&nrm.y, sizeof(FLOAT), 1, f);
		fwrite(&nrm.z, sizeof(FLOAT), 1, f);

		fwrite(&uv.x, sizeof(FLOAT), 1, f);
		fwrite(&uv.y, sizeof(FLOAT), 1, f);
	}
}

VOID TextureName(CONST CHAR* path, CHAR* str)
{
	UINT start = 0;

	for (UINT sp = 0; sp < strlen(path); sp++)
	{
		if (path[sp] == '\\')
		{
			start = sp;
		}
	}

	UINT sc = 0;

	for (UINT sp = start + 1; sp < strlen(path); sp++)
	{
		str[sc] = path[sp];

		sc++;
	}
}
