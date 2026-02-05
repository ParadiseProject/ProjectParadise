// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/InGame/InGameGameState.h"

/**
 * @details ���� �ߺ� üũ �� ���� �����ϸ�, ��������Ʈ�� ���ε��� UI �� �ý��ۿ� ���� ��ȭ�� �˸��ϴ�.
 * UI: ���â ǥ�� �� Ÿ�̸� Ȱ��ȭ
 * ĳ����: ����� ���� �Է� ����(Ready/Result) �� �ִϸ��̼� ���(Victory/Defeat)
 * �ý���: ���� ���� ����/���� �� BGM ��ü
 */
void AInGameGameState::SetCurrentPhase(EGamePhase NewPhase)
{
    // 1. �ߺ� ȣ�� ����
    if (CurrentPhase == NewPhase) return;

    // 2. ���� �� ����
    CurrentPhase = NewPhase;

    // 3. ���� ���� �˸� (UI �� ��Ÿ �ý���)
    if (OnGamePhaseChanged.IsBound())
    {
        OnGamePhaseChanged.Broadcast(CurrentPhase);
    }
}
