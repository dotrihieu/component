#include "Sprite.h"
#include "LoadingManager.h"
#include "Common.h"
#include "Device_Base.h"
#include "Screen.h"
#include "tinyxml2\tinyxml2.h"


void Sprite::InitFromXML(const char* buffer)
{
	//int len = strlen(buffer);
	tinyxml2::XMLDocument doc;
	doc.Parse( buffer );
	if(doc.ErrorID() != 0)
	{
		return;
	}
	tinyxml2::XMLElement* root = doc.FirstChildElement("sprite");
	tinyxml2::XMLElement* image = root->FirstChildElement("image");
	const char* imageName = image->Attribute("name");
	texture = TextureManager::GetInstance()->LoadTexture(imageName);

	tinyxml2::XMLElement* moduleListXML = root->FirstChildElement("moduleList");
	if(moduleListXML)
	{
		tinyxml2::XMLElement* moduleXML = moduleListXML->FirstChildElement("module");
		while(moduleXML)
		{
			SpriteModule* spriteModule = new SpriteModule();
			spriteModule->id = moduleXML->IntAttribute("id");
			spriteModule->width = moduleXML->IntAttribute("w");
			spriteModule->height = moduleXML->IntAttribute("h");

			spriteModule->texcoord.left = moduleXML->FloatAttribute("left_OpenGL");
			spriteModule->texcoord.top = moduleXML->FloatAttribute("top_OpenGL");
			spriteModule->texcoord.right = moduleXML->FloatAttribute("right_OpenGL");
			spriteModule->texcoord.bottom = moduleXML->FloatAttribute("bottom_OpenGL");
			this->moduleList.push_back(spriteModule);
			moduleXML = moduleXML->NextSiblingElement("module");
		}
	}
	tinyxml2::XMLElement* frameListXML = root->FirstChildElement("frameList");
	if(frameListXML)
	{
		tinyxml2::XMLElement* frameXML = frameListXML->FirstChildElement("frame");
		while(frameXML)
		{
			SpriteFrame* spriteFrame = new SpriteFrame();
			spriteFrame->id = frameXML->IntAttribute("id");
			spriteFrame->desc = frameXML->Attribute("desc");
			tinyxml2::XMLElement* frameModuleXML = frameXML->FirstChildElement("frameModule");
			while(frameModuleXML)
			{
				SpriteFrameModule* frameModule = new SpriteFrameModule();
				int id = frameModuleXML->IntAttribute("id");
				for(int i = 0;i<moduleList.size();i++)
				{
					if(moduleList[i]->id == id)
					{
						frameModule->module = moduleList[i];
						break;
					}
				}
				frameModule->x = frameModuleXML->IntAttribute("x");
				frameModule->y = frameModuleXML->IntAttribute("y");
				frameModule->width = frameModuleXML->IntAttribute("w");
				if (frameModule->width == 0)
				{
					frameModule->width = frameModule->module->width;
				}
				frameModule->height = frameModuleXML->IntAttribute("h");
				if (frameModule->height == 0)
				{
					frameModule->height = frameModule->module->height;
				}
				frameModule->flipH = frameModuleXML->BoolAttribute("flipH");
				frameModule->flipV = frameModuleXML->BoolAttribute("flipV");
				spriteFrame->frameModuleList.push_back(frameModule);
				frameModuleXML = frameModuleXML->NextSiblingElement("frameModule");
			}
			tinyxml2::XMLElement* boundGroupXML = frameXML->FirstChildElement("boundGroup");
			
			while(boundGroupXML)
			{
				//boundGroup->id = atoi(boundGroupXML->Attribute("id"));
				const char* desc = boundGroupXML->Attribute("desc");
				BoundGroup* boundGroup = NULL;
				BoundGroup* boundGroupLogical = NULL;
				if(strcmp(desc, "Body") == 0)
				{
					boundGroup = spriteFrame->boundGroups[BoundGroup::BG_TYPE_BODY];
					boundGroupLogical = spriteFrame->boundGroupsLogical[BoundGroup::BG_TYPE_BODY];
				}
				else if(strcmp(desc, "Weapon") == 0)
				{
					boundGroup = spriteFrame->boundGroups[BoundGroup::BG_TYPE_WEAPON];
					boundGroupLogical = spriteFrame->boundGroupsLogical[BoundGroup::BG_TYPE_WEAPON];
				}
				if(boundGroup != NULL)
				{
					tinyxml2::XMLElement* boundXML = boundGroupXML->FirstChildElement("bound");
					while(boundXML)
					{
						const char* type = boundXML->Attribute("type");
						if(strcmp(type, "Rectangle") == 0)
						{
							/*
							BoundRectangle* bound = new BoundRectangle();
							bound->x = atoi(boundXML->Attribute("x"));
							bound->y = atoi(boundXML->Attribute("y"));
							bound->w = atoi(boundXML->Attribute("w"));
							bound->h = atoi(boundXML->Attribute("h"));
							boundGroup->boundList.push_back(bound);
							*/
							float x = boundXML->IntAttribute("x");
							float y = boundXML->IntAttribute("y");
							float w = boundXML->IntAttribute("w");
							float h = boundXML->IntAttribute("h");
							b2PolygonShape* shape = new b2PolygonShape();
							shape->SetAsBox(w/2, h/2, b2Vec2(x + w/2, y + h/2), 0.0);
							boundGroup->shapeList.push_back(shape);
							b2PolygonShape* shapeLogical = new b2PolygonShape();
							float scale = 2.0f / float(Screen::GetInstance()->GetHeight());
							shapeLogical->SetAsBox(w / 2 * scale, h / 2 * scale, b2Vec2((x + w / 2) * scale, (y + h / 2) * scale), 0.0);
							boundGroupLogical->shapeList.push_back(shapeLogical);
						}
						else if(strcmp(type, "Circle") == 0)
						{
							/*
							BoundCircle* bound = new BoundCircle();
							bound->x = atoi(boundXML->Attribute("x"));
							bound->y = atoi(boundXML->Attribute("y"));
							bound->r = atoi(boundXML->Attribute("r"));
							boundGroup->boundList.push_back(bound);
							*/
							float x = boundXML->IntAttribute("x");
							float y = boundXML->IntAttribute("y");
							float r = boundXML->IntAttribute("r");
							b2CircleShape* shape = new b2CircleShape();
							shape->m_p.Set(x, y);
							shape->m_radius = r;
							boundGroup->shapeList.push_back(shape);
							float scale = 2.0f / float(Screen::GetInstance()->GetHeight());
							b2CircleShape* shapeLogical = new b2CircleShape();
							shapeLogical->m_p.Set(x*scale, y*scale);
							shapeLogical->m_radius = r*scale;
							boundGroupLogical->shapeList.push_back(shapeLogical);
						}
						else
						{

						}
						boundXML = boundXML->NextSiblingElement("bound");
					}
				}
				boundGroupXML = boundGroupXML->NextSiblingElement("boundGroup");
			}
			this->frameList.push_back(spriteFrame);
			frameXML = frameXML->NextSiblingElement("frame");
		}
		//------------- init VBO
		vbo = new VBO(VBO::VBO_TARGET_ARRAY_BUFFER);
		vbo->usage = VBO::VBO_USAGE_STATIC_DRAW;
		vbo->SetAttributeOffset(ATTRIBUTE_FLOAT3_POSITION, 0);
		vbo->SetAttributeOffset(ATTRIBUTE_FLOAT2_TEXCOORD_DIFFUSE, 3 * sizeof(float));

		int vboVerticesCount = 0;
		for (std::vector<SpriteFrame*>::iterator it = frameList.begin(); it != frameList.end(); it++)
		{
			(*it)->indexInVBO = vboVerticesCount;
			(*it)->countInVBO = (*it)->frameModuleList.size() * 6;
			vboVerticesCount += (*it)->countInVBO;
		}
		int vboSizeInByte = vboVerticesCount * sizeof(float) * 5; //x, y, z, u, v
		char* vboData = new char[vboSizeInByte];
		float* vboDataPointer = (float*)vboData;
		for (std::vector<SpriteFrame*>::iterator it = frameList.begin(); it != frameList.end(); it++)
		{
			float dz = 1.0f / (float)((*it)->frameModuleList.size() + 1);
			float z = -1.0f;
			for (std::vector<SpriteFrameModule*>::iterator it2 = (*it)->frameModuleList.begin(); it2 != (*it)->frameModuleList.end(); it2++)
			{
				Vector3f pos1, pos2, pos3, pos4;
				Vector2f uv1, uv2, uv3, uv4;
				pos1.x = (*it2)->x;	//top-left
				pos1.y = (*it2)->y + (*it2)->height;
				pos1.z = z;
				uv1.x = (*it2)->flipH == false ? (*it2)->module->texcoord.left : (*it2)->module->texcoord.right;
				uv1.y = (*it2)->flipV == false ? (*it2)->module->texcoord.top : (*it2)->module->texcoord.bottom;
				pos2.x = (*it2)->x;	//bottom-left
				pos2.y = (*it2)->y;
				pos2.z = z;
				uv2.x = (*it2)->flipH == false ? (*it2)->module->texcoord.left : (*it2)->module->texcoord.right;
				uv2.y = (*it2)->flipV == false ? (*it2)->module->texcoord.bottom : (*it2)->module->texcoord.top;
				pos3.x = (*it2)->x + (*it2)->width; //bottom-right
				pos3.y = (*it2)->y;
				pos3.z = z;
				uv3.x = (*it2)->flipH == false ? (*it2)->module->texcoord.right : (*it2)->module->texcoord.left;
				uv3.y = (*it2)->flipV == false ? (*it2)->module->texcoord.bottom : (*it2)->module->texcoord.top;
				pos4.x = (*it2)->x + (*it2)->width;
				pos4.y = (*it2)->y + (*it2)->height;
				pos4.z = z;
				uv4.x = (*it2)->flipH == false ? (*it2)->module->texcoord.right : (*it2)->module->texcoord.left;
				uv4.y = (*it2)->flipV == false ? (*it2)->module->texcoord.top : (*it2)->module->texcoord.bottom;
				
				memcpy(vboDataPointer, &pos1, 3 * sizeof(float));
				vboDataPointer += 3;
				memcpy(vboDataPointer, &uv1, 2 * sizeof(float));
				vboDataPointer += 2;
				memcpy(vboDataPointer, &pos2, 3 * sizeof(float));
				vboDataPointer += 3;
				memcpy(vboDataPointer, &uv2, 2 * sizeof(float));
				vboDataPointer += 2;
				memcpy(vboDataPointer, &pos3, 3 * sizeof(float));
				vboDataPointer += 3;
				memcpy(vboDataPointer, &uv3, 2 * sizeof(float));
				vboDataPointer += 2;
				memcpy(vboDataPointer, &pos1, 3 * sizeof(float));
				vboDataPointer += 3;
				memcpy(vboDataPointer, &uv1, 2 * sizeof(float));
				vboDataPointer += 2;
				memcpy(vboDataPointer, &pos3, 3 * sizeof(float));
				vboDataPointer += 3;
				memcpy(vboDataPointer, &uv3, 2 * sizeof(float));
				vboDataPointer += 2;
				memcpy(vboDataPointer, &pos4, 3 * sizeof(float));
				vboDataPointer += 3;
				memcpy(vboDataPointer, &uv4, 2 * sizeof(float));
				vboDataPointer += 2;
				z += dz;
			}
		}
		Device_Base::GetInstance()->CreateVBO(vbo, vboData, vboSizeInByte);
		SafeDeleteArray(vboData);
		//-------------
	}
	tinyxml2::XMLElement* animListXML = root->FirstChildElement("animList");
	if(animListXML)
	{
		tinyxml2::XMLElement* animXML = animListXML->FirstChildElement("anim");
		while(animXML)
		{
			SpriteAnim* anim = new SpriteAnim();
			anim->desc = animXML->Attribute("desc");
			tinyxml2::XMLElement* animFrameXML = animXML->FirstChildElement("animFrame");
			while(animFrameXML)
			{
				SpriteAnimFrame* animFrame = new SpriteAnimFrame();
				int id = animFrameXML->IntAttribute("id");
				for(int i = 0;i<frameList.size();i++)
				{
					if(frameList[i]->id == id)
					{
						animFrame->frame = frameList[i];
						break;
					}
				}
				animFrame->delay = animFrameXML->FloatAttribute("time")/1000.0f;
				anim->animFrameList.push_back(animFrame);
				animFrameXML = animFrameXML->NextSiblingElement("animFrame");
			}
			anim->id = animXML->IntAttribute("id");
			anim->loop = strcmp(animXML->Attribute("loop"), "true") == 0;
			animList.push_back(anim);
			animXML = animXML->NextSiblingElement("anim");
		}
	}
}
void Sprite::DrawModule(int id, float x, float y)
{
	RectScreenF dest;
	dest.FromWidthHeight(x, y, moduleList[id]->width, moduleList[id]->height);

#ifdef USE_DIRECTX
	Device_Base::GetInstance()->Draw2DTextureScreenCoord(texture, sampler, dest, moduleList[id]->texcoord);
#else
	Device_Base::GetInstance()->Draw2DTextureScreenCoord(texture, dest, moduleList[id]->texcoord);
#endif
}
void Sprite::DrawModuleLogical(int id, float x, float y)
{
	//RectScreenF dest;
	//dest.FromWidthHeight(x, y, Screen::GetInstance()->ScreenToLogical(moduleList[id]->width)
	//	, Screen::GetInstance()->ScreenToLogical(moduleList[id]->height));

#ifdef USE_DIRECTX
	Device_Base::GetInstance()->Draw2DTextureScreenCoord(texture, sampler, dest, moduleList[id]->texcoord);
#else
	//Device_Base::GetInstance()->Draw2DTextureLogicalCoord(texture, dest, moduleList[id]->texcoord);
#endif
}
void Sprite::DrawFrame(int id, float x, float y)
{
	Device_Base::GetInstance()->PushMatrix();
	Device_Base::GetInstance()->Translate(x, y, 0.0f);
	Device_Base::GetInstance()->BindVBO(vbo);
	Device_Base::GetInstance()->UpdateVertexAttribPointer();
	Device_Base::GetInstance()->SetUniformTexture(UNIFORM_TEXTURE_DIFFUSE, this->texture);
	Device_Base::GetInstance()->SetUniformMatrix(UNIFORM_MATRIX_MODEL, Device_Base::GetInstance()->GetMatrixWorld());
	Device_Base::GetInstance()->SetUniformMatrix(UNIFORM_MATRIX_VIEW, Device_Base::GetInstance()->GetMatrixView());
	Device_Base::GetInstance()->SetUniformMatrix(UNIFORM_MATRIX_PROJECTION, Device_Base::GetInstance()->GetMatrixProjection());
	for(int i = 0;i<frameList[id]->frameModuleList.size();i++)
	{
		Device_Base::GetInstance()->DrawArray(Device_Base::PRIMITIVE_TRIANGLE_LIST, frameList[id]->indexInVBO + 6 * i, 6);
	}
	Device_Base::GetInstance()->PopMatrix();
}
void Sprite::DrawFrameLogical(int id, float x, float y)
{
	Device_Base::GetInstance()->PushMatrix();
	Device_Base::GetInstance()->Translate(x, y, 0.0f);
	Device_Base::GetInstance()->Scale(2.0f/float(Screen::GetInstance()->GetHeight()), 2.0f/float(Screen::GetInstance()->GetHeight()), 1.0);
	Device_Base::GetInstance()->BindVBO(vbo);
	Device_Base::GetInstance()->UpdateVertexAttribPointer();
	Device_Base::GetInstance()->SetUniformTexture(UNIFORM_TEXTURE_DIFFUSE, this->texture);
	Device_Base::GetInstance()->SetUniformMatrix(UNIFORM_MATRIX_MODEL, Device_Base::GetInstance()->GetMatrixWorld());
	Device_Base::GetInstance()->SetUniformMatrix(UNIFORM_MATRIX_VIEW, Device_Base::GetInstance()->GetMatrixView());
	Device_Base::GetInstance()->SetUniformMatrix(UNIFORM_MATRIX_PROJECTION, Device_Base::GetInstance()->GetMatrixProjection());
	for (int i = 0; i<frameList[id]->frameModuleList.size(); i++)
	{
		Device_Base::GetInstance()->DrawArray(Device_Base::PRIMITIVE_TRIANGLE_LIST, frameList[id]->indexInVBO + 6 * i, 6);
	}
	Device_Base::GetInstance()->PopMatrix();
}
void Sprite::DrawAnim(SpriteAnimInstance* animInstance, float x, float y)
{
	DrawFrame(animInstance->anim->animFrameList[animInstance->currentFrame]->frame->id, x, y);
}
void Sprite::DrawAnimLogical(SpriteAnimInstance* animInstance, float x, float y)
{
	DrawFrameLogical(animInstance->anim->animFrameList[animInstance->currentFrame]->frame->id, x, y);
}
SpriteAnim* Sprite::GetAnimByIndex(int index)
{
	if (index < 0 || index > animList.size() - 1)
	{
		return NULL;
	}
	return animList[index];
}
SpriteAnim* Sprite::GetAnimByID(int id)
{
	for (std::vector<SpriteAnim*>::iterator it = animList.begin(); it != animList.end(); it++)
	{
		if ((*it)->id == id)
			return *it;
	}
	return NULL;
}
SpriteAnim*Sprite::GetAnimByDesc(const char* desc)
{
	for (std::vector<SpriteAnim*>::iterator it = animList.begin(); it != animList.end(); it++)
	{
		if ((*it)->desc.compare(desc) == 0)
			return *it;
	}
	return NULL;
}
Sprite* SpriteManager::GetSpriteByFileName(char* filename)
{
	for(unsigned int i = 0;i<spriteList.size();i++)
	{
		if(strcmp(spriteList[i]->GetFileName(), filename) == 0)
		{
			return spriteList[i];
		}
	}
	return NULL;
}

Sprite* SpriteManager::LoadSpriteFromXMLFile(char* filename)
{
	Sprite* sprite = GetSpriteByFileName(filename);
	if(sprite != NULL)
	{
		return sprite;
	}
	else
	{
		sprite = new Sprite();
		SafeCopyCHAR(&(sprite->fileName), filename);
		spriteList.push_back(sprite);
		LoadingManager::GetInstance()->AddLoadingSpriteXMLTask(sprite);
		return sprite;
	}
}

void SpriteAnimInstance::Update(float dt)
{
	if (anim->loop == false && finished)
	{
		return;
	}
	time += dt;
	while(time > anim->animFrameList[currentFrame]->delay)
	{
		time -= anim->animFrameList[currentFrame]->delay;
		if(currentFrame < anim->animFrameList.size() - 1)
		{
			currentFrame++;
		}
		else if(anim->loop)
		{
			currentFrame = 0;
		}
		else
		{
			finished = true;
			break;
		}
	}
}
bool SpriteAnimInstance::IsFinished()
{
	return finished;
}