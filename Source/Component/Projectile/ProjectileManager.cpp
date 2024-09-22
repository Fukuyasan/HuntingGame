#include "ProjectileManager.h"

// �X�V����
void ProjectileManager::Update(const float& elapsedTime)
{
    // �X�V����
    for (Projectile* projectile : projectiles)
    {
        projectile->Update(elapsedTime);
    }
    
    // �j������
    Destruction();
}

// �`�揈��
void ProjectileManager::Render(ID3D11DeviceContext* context, Shader* shader)
{
    // �`�揈��
    for (Projectile* projectile : projectiles)
    {
        projectile->Render(context, shader);
    }
}

// �f�o�b�O�v���~�e�B�u�`��
void ProjectileManager::DrawDebugPrimitive()
{
    // �f�o�b�O�v���~�e�B�u�`��
    for (Projectile* projectile : projectiles)
    {
        projectile->DrawDebugPrimitive();
    }
}

// �e�ۓo�^
void ProjectileManager::Register(Projectile* projectile)
{
    // �e�ۓo�^
    projectiles.emplace_back(projectile);
}

// �e�ۑS�폜
void ProjectileManager::Clear()
{
    // �e�ۑS�폜
    for (Projectile* projectile : projectiles)
    {
        delete projectile;
    }
    projectiles.clear();
}

// �e�ۍ폜
void ProjectileManager::Remove(Projectile* projectile)
{
    // �j�����X�g�ɒǉ�
    removes.insert(projectile);

    // ���ځAprojectiles�̗v�f���폜���Ă��܂���
    // �͈�for���ŕs����N�������߁A
    // �j�����X�g�ɒǉ�����
}

void ProjectileManager::Destruction()
{
    // �j������
    // �� projectiles�͈̔�for������erase()����ƕs����������Ă��܂����߁A
    //    �X�V�������I�������ɔj�����X�g�̐ς܂ꂽ�I�u�W�F�N�g���폜����B
    for (Projectile* projectile : removes)
    {
        // std::vector����v�f���폜����ꍇ�̓C�e���[�^�[�ō폜���Ȃ���΂Ȃ�Ȃ�
        std::vector<Projectile*>::iterator it = std::find(projectiles.begin(), projectiles.end(), projectile);

        // std::vector�ŊǗ�����Ă���v�f���폜����ɂ�erase()�֐����g�p����B
        // �j�����X�g�̃|�C���^����C�e���[�^�[���������Aerase()�֐��ɓn���B

        if (it != projectiles.end())
        {
            projectiles.erase(it);
        }

        // �e�ۂ̔j������
        delete projectile;
    }
    // �j�����X�g���N���A
    removes.clear();
}
