#pragma once

#include <vector>
#include <set>

#include "Projectile.h"

// �e�ۃ}�l�[�W���[
class ProjectileManager final 
{
private:
    ProjectileManager()  = default;  // �R���X�g���N�^
    ~ProjectileManager() = default;  // �f�X�g���N�^

public:
    // ���[�u�R���X�g���N�^�A�R�s�[�R���X�g���N�^�� delete �w�肵�Ă���
    ProjectileManager(ProjectileManager&&)      = delete;
    ProjectileManager(const ProjectileManager&) = delete;
    // ���[�u������Z�q�A�R�s�[������Z�q�� delete �w�肵�Ă���
    ProjectileManager operator= (ProjectileManager&&)      = delete;
    ProjectileManager operator= (const ProjectileManager&) = delete;

public:
    // �V���O���g���ɂ͂��Ȃ��Ƃ������ȁA�d�l�ύX���B
    static ProjectileManager& Instance()
    {
        static ProjectileManager instance;
        return instance;
    }

    // �X�V����
    void Update(const float& elapsedTime);

    // �`�揈��
    void Render(ID3D11DeviceContext* context, Shader* shader);

    // �f�o�b�O�v���~�e�B�u�`��
    void DrawDebugPrimitive();

    // �e�ۓo�^
    void Register(Projectile* projectile);

    // �e�ۑS�폜
    void Clear();
    
    // �e�ۍ폜
    void Remove(Projectile* projectile);

#pragma region �Q�b�^�[
    // �e�ې��擾
    int GetProjectileCount() const { return static_cast<int>(projectiles.size()); }

    // �e�ێ擾
    Projectile* GetProjectile(int index) { return projectiles.at(index); }  
#pragma endregion
private:
    // �j������
    void Destruction();

private:
    std::vector<Projectile*> projectiles;
    std::set<Projectile*>    removes;

    // �� std::set�͓����v�f���d�����Ȃ�
    //    std::vector�݂����Ȃ��́B
};
