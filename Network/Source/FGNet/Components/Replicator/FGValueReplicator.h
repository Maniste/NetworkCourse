#include "FGReplicatorBase.h"
#include "FGValueReplicator.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFGOnSmoothValueReplicationChanged);

UCLASS()
class FGNET_API UFGValueReplicator : public UFGReplicatorBase
{
	GENERATED_BODY()
public:
	virtual void Tick(float DeltaTime) override;
	virtual void Init() override;

	UFUNCTION(Server, Reliable)
		void Server_SendTerminalValue(int32 SyncTag, float TerminalValue);

	UFUNCTION(Server, Unreliable)
		void Server_SendReplicatedValue(int32 SyncTag, float ReplicatedValue);

	UFUNCTION(NetMulticast, Reliable)
		void Multicast_SendTerminalValue(int32 SyncTag, float ReplicatedValue);

	UFUNCTION(NetMulticast, Unreliable)
		void Multicast_SendReplicatedValue(int32 SyncTag, float ReplicatedValue);

	UFUNCTION(BlueprintCallable, Category = Network)
		void SetValue(float InValue);

	UFUNCTION(BlueprintPure, Category = Network)
		float GetValue() const;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ClampMin = 1))
		int32 NumberOfReplicationsPerSecond = 5;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		EFGSmoothReplicatorMode SmoothMode = EFGSmoothReplicatorMode::ConstantVelocity;

	UPROPERTY(BlueprintAssignable)
		FFGOnSmoothValueReplicationChanged OnValueChanged;
	
	bool ShouldTick() const;
		
private:
	void BroadcastDelegate();

	struct FCrumb
	{
		float Value;
	};

	TArray<FCrumb, TInlineAllocator<10>> CrumbTail;

	float ReplicatedValueTarget = 0.0f;
	float ReplicatedValueCurrent = 0.f;
	float ReplicatedValuePreviouslySent = 0.0f;
	float StaticValueTimer = 0.0f;
	float SleepAfterDuration = 1.0f;

	int32 NextSyncTag = 0;
	int32 LastReceivedSyncTag = -1;
	int32 LastReceivedCrumbSyncTag = -1;

	float SyncTimer = 0.f;
	float LerpSpeed = 1.f;
	float CurrentCrumbTimeRemaining = 0.f;

	bool bHasReceivedTerminalValue = false;
	bool bHasSentTerminalValue = false;
	bool bIsSleeping = false;

};